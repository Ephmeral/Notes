/* Breadth-first and depth-first routines for
   searching multiple-inheritance lattice for GNU C++.
   Copyright (C) 1987, 89, 92-97, 1998, 1999 Free Software Foundation, Inc.
   Contributed by Michael Tiemann (tiemann@cygnus.com)

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* High-level class interface.  */

#include "config.h"
#include "system.h"
#include "tree.h"
#include "cp-tree.h"
#include "obstack.h"
#include "flags.h"
#include "rtl.h"
#include "output.h"
#include "toplev.h"
#include "varray.h"

#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free

extern struct obstack *current_obstack;
extern tree abort_fndecl;

#include "stack.h"

/* Obstack used for remembering decision points of breadth-first.  */

static struct obstack search_obstack;

/* Methods for pushing and popping objects to and from obstacks.  */

struct stack_level *
push_stack_level (obstack, tp, size)
     struct obstack *obstack;
     char *tp;  /* Sony NewsOS 5.0 compiler doesn't like void * here.  */
     int size;
{
  struct stack_level *stack;
  obstack_grow (obstack, tp, size);
  stack = (struct stack_level *) ((char*)obstack_next_free (obstack) - size);
  obstack_finish (obstack);
  stack->obstack = obstack;
  stack->first = (tree *) obstack_base (obstack);
  stack->limit = obstack_room (obstack) / sizeof (tree *);
  return stack;
}

struct stack_level *
pop_stack_level (stack)
     struct stack_level *stack;
{
  struct stack_level *tem = stack;
  struct obstack *obstack = tem->obstack;
  stack = tem->prev;
  obstack_free (obstack, tem);
  return stack;
}

#define search_level stack_level
static struct search_level *search_stack;

static tree get_abstract_virtuals_1 PROTO((tree, int, tree));
static tree next_baselink PROTO((tree));
static tree get_vbase_1 PROTO((tree, tree, unsigned int *));
static tree convert_pointer_to_vbase PROTO((tree, tree));
static tree lookup_field_1 PROTO((tree, tree));
static tree convert_pointer_to_single_level PROTO((tree, tree));
static int lookup_fnfields_here PROTO((tree, tree));
static int is_subobject_of_p PROTO((tree, tree));
static int hides PROTO((tree, tree));
static tree virtual_context PROTO((tree, tree, tree));
static tree dfs_check_overlap PROTO((tree, void *));
static tree dfs_no_overlap_yet PROTO((tree, void *));
static int get_base_distance_recursive
	PROTO((tree, int, int, int, int *, tree *, tree,
	       int, int *, int, int));
static void expand_upcast_fixups 
	PROTO((tree, tree, tree, tree, tree, tree, tree *));
static void fixup_virtual_upcast_offsets
	PROTO((tree, tree, int, int, tree, tree, tree, tree,
	       tree *));
static tree unmarkedp PROTO((tree, void *));
static tree marked_vtable_pathp PROTO((tree, void *));
static tree unmarked_vtable_pathp PROTO((tree, void *));
static tree marked_new_vtablep PROTO((tree, void *));
static tree unmarked_new_vtablep PROTO((tree, void *));
static tree marked_pushdecls_p PROTO((tree, void *));
static tree unmarked_pushdecls_p PROTO((tree, void *));
static tree dfs_debug_unmarkedp PROTO((tree, void *));
static tree dfs_debug_mark PROTO((tree, void *));
static tree dfs_find_vbases PROTO((tree, void *));
static tree dfs_clear_vbase_slots PROTO((tree, void *));
static tree dfs_init_vbase_pointers PROTO((tree, void *));
static tree dfs_get_vbase_types PROTO((tree, void *));
static tree dfs_push_type_decls PROTO((tree, void *));
static tree dfs_push_decls PROTO((tree, void *));
static tree dfs_unuse_fields PROTO((tree, void *));
static tree add_conversions PROTO((tree, void *));
static tree get_virtuals_named_this PROTO((tree, tree));
static tree get_virtual_destructor PROTO((tree, void *));
static tree tree_has_any_destructor_p PROTO((tree, void *));
static int covariant_return_p PROTO((tree, tree));
static struct search_level *push_search_level
	PROTO((struct stack_level *, struct obstack *));
static struct search_level *pop_search_level
	PROTO((struct stack_level *));
static tree bfs_walk
	PROTO((tree, tree (*) (tree, void *), tree (*) (tree, void *),
	       void *));
static tree lookup_field_queue_p PROTO((tree, void *));
static tree lookup_field_r PROTO((tree, void *));
static tree dfs_walk_real PROTO ((tree, 
				  tree (*) (tree, void *),
				  tree (*) (tree, void *),
				  tree (*) (tree, void *),
				  void *));
static tree dfs_bfv_queue_p PROTO ((tree, void *));
static tree dfs_bfv_helper PROTO ((tree, void *));
static tree get_virtuals_named_this_r PROTO ((tree, void *));
static tree context_for_name_lookup PROTO ((tree));
static tree canonical_binfo PROTO ((tree));
static tree shared_marked_p PROTO ((tree, void *));
static tree shared_unmarked_p PROTO ((tree, void *));
static int  dependent_base_p PROTO ((tree));
static tree dfs_accessible_queue_p PROTO ((tree, void *));
static tree dfs_accessible_p PROTO ((tree, void *));
static tree dfs_access_in_type PROTO ((tree, void *));
static tree access_in_type PROTO ((tree, tree));
static tree dfs_canonical_queue PROTO ((tree, void *));
static tree dfs_assert_unmarked_p PROTO ((tree, void *));
static void assert_canonical_unmarked PROTO ((tree));
static int protected_accessible_p PROTO ((tree, tree, tree, tree));
static int friend_accessible_p PROTO ((tree, tree, tree, tree));
static void setup_class_bindings PROTO ((tree, int));
static int template_self_reference_p PROTO ((tree, tree));

/* Allocate a level of searching.  */

static struct search_level *
push_search_level (stack, obstack)
     struct stack_level *stack;
     struct obstack *obstack;
{
  struct search_level tem;

  tem.prev = stack;
  return push_stack_level (obstack, (char *)&tem, sizeof (tem));
}

/* Discard a level of search allocation.  */

static struct search_level *
pop_search_level (obstack)
     struct stack_level *obstack;
{
  register struct search_level *stack = pop_stack_level (obstack);

  return stack;
}

static tree _vptr_name;

/* Variables for gathering statistics.  */
#ifdef GATHER_STATISTICS
static int n_fields_searched;
static int n_calls_lookup_field, n_calls_lookup_field_1;
static int n_calls_lookup_fnfields, n_calls_lookup_fnfields_1;
static int n_calls_get_base_type;
static int n_outer_fields_searched;
static int n_contexts_saved;
#endif /* GATHER_STATISTICS */


/* Get a virtual binfo that is found inside BINFO's hierarchy that is
   the same type as the type given in PARENT.  To be optimal, we want
   the first one that is found by going through the least number of
   virtual bases.

   This uses a clever algorithm that updates *depth when we find the vbase,
   and cuts off other paths of search when they reach that depth.  */

static tree
get_vbase_1 (parent, binfo, depth)
     tree parent, binfo;
     unsigned int *depth;
{
  tree binfos;
  int i, n_baselinks;
  tree rval = NULL_TREE;

  if (BINFO_TYPE (binfo) == parent && TREE_VIA_VIRTUAL (binfo))
    {
      *depth = 0;
      return binfo;
    }

  *depth = *depth - 1;

  binfos = BINFO_BASETYPES (binfo);
  n_baselinks = binfos ? TREE_VEC_LENGTH (binfos) : 0;

  /* Process base types.  */
  for (i = 0; i < n_baselinks; i++)
    {
      tree base_binfo = TREE_VEC_ELT (binfos, i);
      tree nrval;

      if (*depth == 0)
	break;

      nrval = get_vbase_1 (parent, base_binfo, depth);
      if (nrval)
	rval = nrval;
    }
  *depth = *depth+1;
  return rval;
}

/* Return the shortest path to vbase PARENT within BINFO, ignoring
   access and ambiguity.  */

tree
get_vbase (parent, binfo)
     tree parent;
     tree binfo;
{
  unsigned int d = (unsigned int)-1;
  return get_vbase_1 (parent, binfo, &d);
}

/* Convert EXPR to a virtual base class of type TYPE.  We know that
   EXPR is a non-null POINTER_TYPE to RECORD_TYPE.  We also know that
   the type of what expr points to has a virtual base of type TYPE.  */

static tree
convert_pointer_to_vbase (type, expr)
     tree type;
     tree expr;
{
  tree vb = get_vbase (type, TYPE_BINFO (TREE_TYPE (TREE_TYPE (expr))));
  return convert_pointer_to_real (vb, expr);
}

/* Check whether the type given in BINFO is derived from PARENT.  If
   it isn't, return 0.  If it is, but the derivation is MI-ambiguous
   AND protect != 0, emit an error message and return error_mark_node.

   Otherwise, if TYPE is derived from PARENT, return the actual base
   information, unless a one of the protection violations below
   occurs, in which case emit an error message and return error_mark_node.

   If PROTECT is 1, then check if access to a public field of PARENT
   would be private.  Also check for ambiguity.  */

tree
get_binfo (parent, binfo, protect)
     register tree parent, binfo;
     int protect;
{
  tree type = NULL_TREE;
  int dist;
  tree rval = NULL_TREE;
  
  if (TREE_CODE (parent) == TREE_VEC)
    parent = BINFO_TYPE (parent);
  else if (! IS_AGGR_TYPE_CODE (TREE_CODE (parent)))
    my_friendly_abort (89);

  if (TREE_CODE (binfo) == TREE_VEC)
    type = BINFO_TYPE (binfo);
  else if (IS_AGGR_TYPE_CODE (TREE_CODE (binfo)))
    type = binfo;
  else
    my_friendly_abort (90);
  
  dist = get_base_distance (parent, binfo, protect, &rval);

  if (dist == -3)
    {
      cp_error ("fields of `%T' are inaccessible in `%T' due to private inheritance",
		parent, type);
      return error_mark_node;
    }
  else if (dist == -2 && protect)
    {
      cp_error ("type `%T' is ambiguous base class for type `%T'", parent,
		type);
      return error_mark_node;
    }

  return rval;
}

/* This is the newer depth first get_base_distance routine.  */

static int
get_base_distance_recursive (binfo, depth, is_private, rval,
			     rval_private_ptr, new_binfo_ptr, parent,
			     protect, via_virtual_ptr, via_virtual,
			     current_scope_in_chain)
     tree binfo;
     int depth, is_private, rval;
     int *rval_private_ptr;
     tree *new_binfo_ptr, parent;
     int protect, *via_virtual_ptr, via_virtual;
     int current_scope_in_chain;
{
  tree binfos;
  int i, n_baselinks;

  if (protect
      && !current_scope_in_chain
      && is_friend (BINFO_TYPE (binfo), current_scope ()))
    current_scope_in_chain = 1;

  if (BINFO_TYPE (binfo) == parent || binfo == parent)
    {
      int better = 0;

      if (rval == -1)
	/* This is the first time we've found parent.  */
	better = 1;
      else if (tree_int_cst_equal (BINFO_OFFSET (*new_binfo_ptr),
				   BINFO_OFFSET (binfo))
	       && *via_virtual_ptr && via_virtual)
	{
	  /* A new path to the same vbase.  If this one has better
	     access or is shorter, take it.  */

	  if (protect)
	    better = *rval_private_ptr - is_private;
	  if (better == 0)
	    better = rval - depth;
	}
      else
	{
	  /* Ambiguous base class.  */
	  rval = depth = -2;

	  /* If we get an ambiguity between virtual and non-virtual base
	     class, return the non-virtual in case we are ignoring
	     ambiguity.  */
	  better = *via_virtual_ptr - via_virtual;
	}

      if (better > 0)
	{
	  rval = depth;
	  *rval_private_ptr = is_private;
	  *new_binfo_ptr = binfo;
	  *via_virtual_ptr = via_virtual;
	}

      return rval;
    }

  binfos = BINFO_BASETYPES (binfo);
  n_baselinks = binfos ? TREE_VEC_LENGTH (binfos) : 0;
  depth += 1;

  /* Process base types.  */
  for (i = 0; i < n_baselinks; i++)
    {
      tree base_binfo = TREE_VEC_ELT (binfos, i);

      int via_private
	= (protect
	   && (is_private
	       || (!TREE_VIA_PUBLIC (base_binfo)
		   && !(TREE_VIA_PROTECTED (base_binfo)
			&& current_scope_in_chain)
		   && !is_friend (BINFO_TYPE (binfo), current_scope ()))));
      int this_virtual = via_virtual || TREE_VIA_VIRTUAL (base_binfo);

      rval = get_base_distance_recursive (base_binfo, depth, via_private,
					  rval, rval_private_ptr,
					  new_binfo_ptr, parent,
					  protect, via_virtual_ptr,
					  this_virtual,
					  current_scope_in_chain);

      /* If we've found a non-virtual, ambiguous base class, we don't need
	 to keep searching.  */
      if (rval == -2 && *via_virtual_ptr == 0)
	return rval;
    }

  return rval;
}

/* Return the number of levels between type PARENT and the type given
   in BINFO, following the leftmost path to PARENT not found along a
   virtual path, if there are no real PARENTs (all come from virtual
   base classes), then follow the shortest public path to PARENT.

   Return -1 if TYPE is not derived from PARENT.
   Return -2 if PARENT is an ambiguous base class of TYPE, and PROTECT is
    non-negative.
   Return -3 if PARENT is private to TYPE, and PROTECT is non-zero.

   If PATH_PTR is non-NULL, then also build the list of types
   from PARENT to TYPE, with TREE_VIA_VIRTUAL and TREE_VIA_PUBLIC
   set.

   PARENT can also be a binfo, in which case that exact parent is found
   and no other.  convert_pointer_to_real uses this functionality.

   If BINFO is a binfo, its BINFO_INHERITANCE_CHAIN will be left alone.  */

int
get_base_distance (parent, binfo, protect, path_ptr)
     register tree parent, binfo;
     int protect;
     tree *path_ptr;
{
  int rval;
  int rval_private = 0;
  tree type = NULL_TREE;
  tree new_binfo = NULL_TREE;
  int via_virtual;
  int watch_access = protect;

  /* Should we be completing types here?  */
  if (TREE_CODE (parent) != TREE_VEC)
    parent = complete_type (TYPE_MAIN_VARIANT (parent));
  else
    complete_type (TREE_TYPE (parent));

  if (TREE_CODE (binfo) == TREE_VEC)
    type = BINFO_TYPE (binfo);
  else if (IS_AGGR_TYPE_CODE (TREE_CODE (binfo)))
    {
      type = complete_type (binfo);
      binfo = TYPE_BINFO (type);

      if (path_ptr)
	my_friendly_assert (BINFO_INHERITANCE_CHAIN (binfo) == NULL_TREE,
			    980827);
    }
  else
    my_friendly_abort (92);

  if (parent == type || parent == binfo)
    {
      /* If the distance is 0, then we don't really need
	 a path pointer, but we shouldn't let garbage go back.  */
      if (path_ptr)
	*path_ptr = binfo;
      return 0;
    }

  if (path_ptr)
    watch_access = 1;

  rval = get_base_distance_recursive (binfo, 0, 0, -1,
				      &rval_private, &new_binfo, parent,
				      watch_access, &via_virtual, 0,
				      0);

  /* Access restrictions don't count if we found an ambiguous basetype.  */
  if (rval == -2 && protect >= 0)
    rval_private = 0;

  if (rval && protect && rval_private)
    return -3;

  /* If they gave us the real vbase binfo, which isn't in the main binfo
     tree, deal with it.  This happens when we are called from
     expand_upcast_fixups.  */
  if (rval == -1 && TREE_CODE (parent) == TREE_VEC
      && parent == binfo_member (BINFO_TYPE (parent),
				 CLASSTYPE_VBASECLASSES (type)))
    {
      my_friendly_assert (BINFO_INHERITANCE_CHAIN (parent) == binfo, 980827);
      new_binfo = parent;
      rval = 1;
    }

  if (path_ptr)
    *path_ptr = new_binfo;
  return rval;
}

/* Search for a member with name NAME in a multiple inheritance lattice
   specified by TYPE.  If it does not exist, return NULL_TREE.
   If the member is ambiguously referenced, return `error_mark_node'.
   Otherwise, return the FIELD_DECL.  */

/* Do a 1-level search for NAME as a member of TYPE.  The caller must
   figure out whether it can access this field.  (Since it is only one
   level, this is reasonable.)  */

static tree
lookup_field_1 (type, name)
     tree type, name;
{
  register tree field;

  if (TREE_CODE (type) == TEMPLATE_TYPE_PARM
      || TREE_CODE (type) == TEMPLATE_TEMPLATE_PARM)
    /* The TYPE_FIELDS of a TEMPLATE_TYPE_PARM are not fields at all;
       instead TYPE_FIELDS is the TEMPLATE_PARM_INDEX.  (Miraculously,
       the code often worked even when we treated the index as a list
       of fields!)  */
    return NULL_TREE;

  field = TYPE_FIELDS (type);

#ifdef GATHER_STATISTICS
  n_calls_lookup_field_1++;
#endif /* GATHER_STATISTICS */
  while (field)
    {
#ifdef GATHER_STATISTICS
      n_fields_searched++;
#endif /* GATHER_STATISTICS */
      my_friendly_assert (TREE_CODE_CLASS (TREE_CODE (field)) == 'd', 0);
      if (DECL_NAME (field) == NULL_TREE
	  && TREE_CODE (TREE_TYPE (field)) == UNION_TYPE)
	{
	  tree temp = lookup_field_1 (TREE_TYPE (field), name);
	  if (temp)
	    return temp;
	}
      if (TREE_CODE (field) == USING_DECL)
	/* For now, we're just treating member using declarations as
	   old ARM-style access declarations.  Thus, there's no reason
	   to return a USING_DECL, and the rest of the compiler can't
	   handle it.  Once the class is defined, these are purged
	   from TYPE_FIELDS anyhow; see handle_using_decl.  */
	;
      else if (DECL_NAME (field) == name)
	{
	  if ((TREE_CODE(field) == VAR_DECL || TREE_CODE(field) == CONST_DECL)
	      && DECL_ASSEMBLER_NAME (field) != NULL)
	    GNU_xref_ref(current_function_decl,
			 IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (field)));
	  return field;
	}
      field = TREE_CHAIN (field);
    }
  /* Not found.  */
  if (name == _vptr_name)
    {
      /* Give the user what s/he thinks s/he wants.  */
      if (TYPE_VIRTUAL_P (type))
	return CLASSTYPE_VFIELD (type);
    }
  return NULL_TREE;
}

/* There are a number of cases we need to be aware of here:
			 current_class_type	current_function_decl
     global			NULL			NULL
     fn-local			NULL			SET
     class-local		SET			NULL
     class->fn			SET			SET
     fn->class			SET			SET

   Those last two make life interesting.  If we're in a function which is
   itself inside a class, we need decls to go into the fn's decls (our
   second case below).  But if we're in a class and the class itself is
   inside a function, we need decls to go into the decls for the class.  To
   achieve this last goal, we must see if, when both current_class_ptr and
   current_function_decl are set, the class was declared inside that
   function.  If so, we know to put the decls into the class's scope.  */

tree
current_scope ()
{
  if (current_function_decl == NULL_TREE)
    return current_class_type;
  if (current_class_type == NULL_TREE)
    return current_function_decl;
  if (DECL_CLASS_CONTEXT (current_function_decl) == current_class_type)
    return current_function_decl;

  return current_class_type;
}

/* Return the scope of DECL, as appropriate when doing name-lookup.  */

static tree
context_for_name_lookup (decl)
     tree decl;
{
  /* [class.union]
     
     For the purposes of name lookup, after the anonymous union
     definition, the members of the anonymous union are considered to
     have been defined in the scope in which teh anonymous union is
     declared.  */ 
  tree context = DECL_REAL_CONTEXT (decl);

  while (TYPE_P (context) && ANON_UNION_TYPE_P (context))
    context = TYPE_CONTEXT (context);
  if (!context)
    context = global_namespace;

  return context;
}

/* Return a canonical BINFO if BINFO is a virtual base, or just BINFO
   otherwise.  */

static tree
canonical_binfo (binfo)
     tree binfo;
{
  return (TREE_VIA_VIRTUAL (binfo)
	  ? TYPE_BINFO (BINFO_TYPE (binfo)) : binfo);
}

/* A queue function that simply ensures that we walk into the
   canonical versions of virtual bases.  */

static tree
dfs_canonical_queue (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  return canonical_binfo (binfo);
}

/* Called via dfs_walk from assert_canonical_unmarked.  */

static tree
dfs_assert_unmarked_p (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  my_friendly_assert (!BINFO_MARKED (binfo), 0);
  return NULL_TREE;
}

/* Asserts that all the nodes below BINFO (using the canonical
   versions of virtual bases) are unmarked.  */

static void
assert_canonical_unmarked (binfo)
     tree binfo;
{
  dfs_walk (binfo, dfs_assert_unmarked_p, dfs_canonical_queue, 0);
}

/* If BINFO is marked, return a canonical version of BINFO.
   Otherwise, return NULL_TREE.  */

static tree
shared_marked_p (binfo, data)
     tree binfo;
     void *data;
{
  binfo = canonical_binfo (binfo);
  return markedp (binfo, data) ? binfo : NULL_TREE;
}

/* If BINFO is not marked, return a canonical version of BINFO.
   Otherwise, return NULL_TREE.  */

static tree
shared_unmarked_p (binfo, data)
     tree binfo;
     void *data;
{
  binfo = canonical_binfo (binfo);
  return unmarkedp (binfo, data) ? binfo : NULL_TREE;
}

/* Called from access_in_type via dfs_walk.  Calculate the access to
   DATA (which is really a DECL) in BINFO.  */

static tree
dfs_access_in_type (binfo, data)
     tree binfo;
     void *data;
{
  tree decl = (tree) data;
  tree type = BINFO_TYPE (binfo);
  tree access = NULL_TREE;

  if (context_for_name_lookup (decl) == type)
    {
      /* If we have desceneded to the scope of DECL, just note the
	 appropriate access.  */
      if (TREE_PRIVATE (decl))
	access = access_private_node;
      else if (TREE_PROTECTED (decl))
	access = access_protected_node;
      else
	access = access_public_node;
    }
  else 
    {
      /* First, check for an access-declaration that gives us more
	 access to the DECL.  The CONST_DECL for an enumeration
	 constant will not have DECL_LANG_SPECIFIC, and thus no
	 DECL_ACCESS.  */
      if (DECL_LANG_SPECIFIC (decl))
	{
	  access = purpose_member (type, DECL_ACCESS (decl));
	  if (access)
	    access = TREE_VALUE (access);
	}

      if (!access)
	{
	  int i;
	  int n_baselinks;
	  tree binfos;
	  
	  /* Otherwise, scan our baseclasses, and pick the most favorable
	     access.  */
	  binfos = BINFO_BASETYPES (binfo);
	  n_baselinks = binfos ? TREE_VEC_LENGTH (binfos) : 0;
	  for (i = 0; i < n_baselinks; ++i)
	    {
	      tree base_binfo = TREE_VEC_ELT (binfos, i);
	      tree base_access = TREE_CHAIN (canonical_binfo (base_binfo));

	      if (!base_access || base_access == access_private_node)
		/* If it was not accessible in the base, or only
		   accessible as a private member, we can't access it
		   all.  */
		base_access = NULL_TREE;
	      else if (TREE_VIA_PROTECTED (base_binfo))
		/* Public and protected members in the base are
		   protected here.  */
		base_access = access_protected_node;
	      else if (!TREE_VIA_PUBLIC (base_binfo))
		/* Public and protected members in the base are
		   private here.  */
		base_access = access_private_node;

	      /* See if the new access, via this base, gives more
		 access than our previous best access.  */
	      if (base_access &&
		  (base_access == access_public_node
		   || (base_access == access_protected_node
		       && access != access_public_node)
		   || (base_access == access_private_node
		       && !access)))
		{
		  access = base_access;

		  /* If the new access is public, we can't do better.  */
		  if (access == access_public_node)
		    break;
		}
	    }
	}
    }

  /* Note the access to DECL in TYPE.  */
  TREE_CHAIN (binfo) = access;

  /* Mark TYPE as visited so that if we reach it again we do not
     duplicate our efforts here.  */
  SET_BINFO_MARKED (binfo);

  return NULL_TREE;
}

/* Return the access to DECL in TYPE.  */

static tree 
access_in_type (type, decl)
     tree type;
     tree decl;
{
  tree binfo = TYPE_BINFO (type);

  /* We must take into account

       [class.paths]

       If a name can be reached by several paths through a multiple
       inheritance graph, the access is that of the path that gives
       most access.  

    The algorithm we use is to make a post-order depth-first traversal
    of the base-class hierarchy.  As we come up the tree, we annotate
    each node with the most lenient access.  */
  dfs_walk_real (binfo, 0, dfs_access_in_type, shared_unmarked_p, decl);
  dfs_walk (binfo, dfs_unmark, shared_marked_p,  0);
  assert_canonical_unmarked (binfo);

  return TREE_CHAIN (binfo);
}

/* Called from dfs_accessible_p via dfs_walk.  */

static tree
dfs_accessible_queue_p (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  if (BINFO_MARKED (binfo))
    return NULL_TREE;

  /* If this class is inherited via private or protected inheritance,
     then we can't see it, unless we are a friend of the subclass.  */
  if (!TREE_VIA_PUBLIC (binfo)
      && !is_friend (BINFO_TYPE (BINFO_INHERITANCE_CHAIN (binfo)),
		     current_scope ()))
    return NULL_TREE;

  return canonical_binfo (binfo);
}

/* Called from dfs_accessible_p via dfs_walk.  */

static tree
dfs_accessible_p (binfo, data)
     tree binfo;
     void *data;
{
  int protected_ok = data != 0;
  tree access;

  /* We marked the binfos while computing the access in each type.
     So, we unmark as we go now.  */
  SET_BINFO_MARKED (binfo);

  access = TREE_CHAIN (binfo);
  if (access == access_public_node
      || (access == access_protected_node && protected_ok))
    return binfo;
  else if (access && is_friend (BINFO_TYPE (binfo), current_scope ()))
    return binfo;

  return NULL_TREE;
}

/* Returns non-zero if it is OK to access DECL when named in TYPE
   through an object indiated by BINFO in the context of DERIVED.  */

static int
protected_accessible_p (type, decl, derived, binfo)
     tree type;
     tree decl;
     tree derived;
     tree binfo;
{
  tree access;

  /* We're checking this clause from [class.access.base]

       m as a member of N is protected, and the reference occurs in a
       member or friend of class N, or in a member or friend of a
       class P derived from N, where m as a member of P is private or
       protected.  

    If DERIVED isn't derived from TYPE, then it certainly does not
    apply.  */
  if (!DERIVED_FROM_P (type, derived))
    return 0;

  access = access_in_type (derived, decl);
  if (same_type_p (derived, type))
    {
      if (access != access_private_node)
	return 0;
    }
  else if (access != access_private_node
	   && access != access_protected_node)
    return 0;
  
  /* [class.protected]

     When a friend or a member function of a derived class references
     a protected nonstatic member of a base class, an access check
     applies in addition to those described earlier in clause
     _class.access_.4) Except when forming a pointer to member
     (_expr.unary.op_), the access must be through a pointer to,
     reference to, or object of the derived class itself (or any class
     derived from that class) (_expr.ref_).  If the access is to form
     a pointer to member, the nested-name-specifier shall name the
     derived class (or any class derived from that class).  */
  if (DECL_NONSTATIC_MEMBER_P (decl))
    {
      /* We can tell through what the reference is occurring by
	 chasing BINFO up to the root.  */
      tree t = binfo;
      while (BINFO_INHERITANCE_CHAIN (t))
	t = BINFO_INHERITANCE_CHAIN (t);
      
      if (!DERIVED_FROM_P (derived, BINFO_TYPE (t)))
	return 0;
    }

  return 1;
}

/* Returns non-zero if SCOPE is a friend of a type which would be able
   to acces DECL, named in TYPE, through the object indicated by
   BINFO.  */

static int
friend_accessible_p (scope, type, decl, binfo)
     tree scope;
     tree type;
     tree decl;
     tree binfo;
{
  tree befriending_classes;
  tree t;

  if (!scope)
    return 0;

  if (TREE_CODE (scope) == FUNCTION_DECL
      || DECL_FUNCTION_TEMPLATE_P (scope))
    befriending_classes = DECL_BEFRIENDING_CLASSES (scope);
  else if (TYPE_P (scope))
    befriending_classes = CLASSTYPE_BEFRIENDING_CLASSES (scope);
  else
    return 0;

  for (t = befriending_classes; t; t = TREE_CHAIN (t))
    if (protected_accessible_p (type, decl, TREE_VALUE (t), binfo))
      return 1;

  if (TREE_CODE (scope) == FUNCTION_DECL
      || DECL_FUNCTION_TEMPLATE_P (scope))
    {
      /* Perhaps this SCOPE is a member of a class which is a 
	 friend.  */ 
      if (friend_accessible_p (DECL_CLASS_CONTEXT (scope), type,
			       decl, binfo))
	return 1;

      /* Or an instantiation of something which is a friend.  */
      if (DECL_TEMPLATE_INFO (scope))
	return friend_accessible_p (DECL_TI_TEMPLATE (scope),
				    type, decl, binfo);
    }
  else if (CLASSTYPE_TEMPLATE_INFO (scope))
    return friend_accessible_p (CLASSTYPE_TI_TEMPLATE (scope),
				type, decl, binfo);

  return 0;
}
   
/* DECL is a declaration from a base class of TYPE, which was the
   classs used to name DECL.  Return non-zero if, in the current
   context, DECL is accessible.  If TYPE is actually a BINFO node,
   then we can tell in what context the access is occurring by looking
   at the most derived class along the path indicated by BINFO.  */

int 
accessible_p (type, decl)
     tree type;
     tree decl;
     
{
  tree binfo;
  tree t;

  /* Non-zero if it's OK to access DECL if it has protected
     accessibility in TYPE.  */
  int protected_ok = 0;

  /* If we're not checking access, everything is accessible.  */
  if (!flag_access_control)
    return 1;

  /* If this declaration is in a block or namespace scope, there's no
     access control.  */
  if (!TYPE_P (context_for_name_lookup (decl)))
    return 1;

  /* We don't do access control for types yet.  */
  if (TREE_CODE (decl) == TYPE_DECL)
    return 1;

  if (!TYPE_P (type))
    {
      binfo = type;
      type = BINFO_TYPE (type);
    }
  else
    binfo = TYPE_BINFO (type);

  /* [class.access.base]

     A member m is accessible when named in class N if

     --m as a member of N is public, or

     --m as a member of N is private, and the reference occurs in a
       member or friend of class N, or

     --m as a member of N is protected, and the reference occurs in a
       member or friend of class N, or in a member or friend of a
       class P derived from N, where m as a member of P is private or
       protected, or

     --there exists a base class B of N that is accessible at the point
       of reference, and m is accessible when named in class B.  

    We walk the base class hierarchy, checking these conditions.  */

  /* Figure out where the reference is occurring.  Check to see if
     DECL is private or protected in this scope, since that will
     determine whether protected access in TYPE allowed.  */
  if (current_class_type)
    protected_ok 
      = protected_accessible_p (type, decl, current_class_type,
				binfo);

  /* Now, loop through the classes of which we are a friend.  */
  if (!protected_ok)
    protected_ok = friend_accessible_p (current_scope (),
					type, decl, binfo);

  /* Standardize on the same that will access_in_type will use.  We
     don't need to know what path was chosen from this point onwards.  */ 
  binfo = TYPE_BINFO (type);

  /* Compute the accessibility of DECL in the class hierarchy
     dominated by type.  */
  access_in_type (type, decl);
  /* Walk the hierarchy again, looking for a base class that allows
     access.  */
  t = dfs_walk (binfo, dfs_accessible_p, 
		dfs_accessible_queue_p,
		protected_ok ? &protected_ok : 0);
  /* Clear any mark bits.  Note that we have to walk the whole tree
     here, since we have aborted the previous walk from some point
     deep in the tree.  */
  dfs_walk (binfo, dfs_unmark, dfs_canonical_queue,  0);
  assert_canonical_unmarked (binfo);

  return t != NULL_TREE;
}

/* Routine to see if the sub-object denoted by the binfo PARENT can be
   found as a base class and sub-object of the object denoted by
   BINFO.  This routine relies upon binfos not being shared, except
   for binfos for virtual bases.  */

static int
is_subobject_of_p (parent, binfo)
     tree parent, binfo;
{
  tree binfos;
  int i, n_baselinks;

  /* We want to canonicalize for comparison purposes.  But, when we
     iterate through basetypes later, we want the binfos from the
     original hierarchy.  That's why we have to calculate BINFOS
     first, and then canonicalize.  */
  binfos = BINFO_BASETYPES (binfo);
  parent = canonical_binfo (parent);
  binfo = canonical_binfo (binfo);

  if (parent == binfo)
    return 1;

  n_baselinks = binfos ? TREE_VEC_LENGTH (binfos) : 0;

  /* Process and/or queue base types.  */
  for (i = 0; i < n_baselinks; i++)
    {
      tree base_binfo = TREE_VEC_ELT (binfos, i);
      if (!CLASS_TYPE_P (TREE_TYPE (base_binfo)))
	/* If we see a TEMPLATE_TYPE_PARM, or some such, as a base
	   class there's no way to descend into it.  */
	continue;

      if (is_subobject_of_p (parent, base_binfo))
	return 1;
    }
  return 0;
}

/* See if a one FIELD_DECL hides another.  This routine is meant to
   correspond to ANSI working paper Sept 17, 1992 10p4.  The two
   binfos given are the binfos corresponding to the particular places
   the FIELD_DECLs are found.  This routine relies upon binfos not
   being shared, except for virtual bases.  */

static int
hides (hider_binfo, hidee_binfo)
     tree hider_binfo, hidee_binfo;
{
  /* hider hides hidee, if hider has hidee as a base class and
     the instance of hidee is a sub-object of hider.  The first
     part is always true is the second part is true.

     When hider and hidee are the same (two ways to get to the exact
     same member) we consider either one as hiding the other.  */
  return is_subobject_of_p (hidee_binfo, hider_binfo);
}

/* Very similar to lookup_fnfields_1 but it ensures that at least one
   function was declared inside the class given by TYPE.  It really should
   only return functions that match the given TYPE.  */

static int
lookup_fnfields_here (type, name)
     tree type, name;
{
  int idx = lookup_fnfields_1 (type, name);
  tree fndecls;

  /* ctors and dtors are always only in the right class.  */
  if (idx <= 1)
    return idx;
  fndecls = TREE_VEC_ELT (CLASSTYPE_METHOD_VEC (type), idx);
  while (fndecls)
    {
      if (TYPE_MAIN_VARIANT (DECL_CLASS_CONTEXT (OVL_CURRENT (fndecls)))
	  == TYPE_MAIN_VARIANT (type))
	return idx;
      fndecls = OVL_CHAIN (fndecls);
    }
  return -1;
}

struct lookup_field_info {
  /* The type in which we're looking.  */
  tree type;
  /* The name of the field for which we're looking.  */
  tree name;
  /* If non-NULL, the current result of the lookup.  */
  tree rval;
  /* The path to RVAL.  */
  tree rval_binfo;
  /* If non-NULL, the lookup was ambiguous, and this is a list of the
     candidates.  */
  tree ambiguous;
  /* If non-zero, we are looking for types, not data members.  */
  int want_type;
  /* If non-zero, RVAL was found by looking through a dependent base.  */
  int from_dep_base_p;
  /* If something went wrong, a message indicating what.  */
  const char *errstr;
};

/* Returns non-zero if BINFO is not hidden by the value found by the
   lookup so far.  If BINFO is hidden, then there's no need to look in
   it.  DATA is really a struct lookup_field_info.  Called from
   lookup_field via breadth_first_search.  */

static tree
lookup_field_queue_p (binfo, data)
     tree binfo;
     void *data;
{
  struct lookup_field_info *lfi = (struct lookup_field_info *) data;

  /* Don't look for constructors or destructors in base classes.  */
  if (lfi->name == ctor_identifier || lfi->name == dtor_identifier)
    return NULL_TREE;

  /* If this base class is hidden by the best-known value so far, we
     don't need to look.  */
  if (!lfi->from_dep_base_p && lfi->rval_binfo
      && hides (lfi->rval_binfo, binfo))
    return NULL_TREE;

  if (TREE_VIA_VIRTUAL (binfo))
    return binfo_member (BINFO_TYPE (binfo),
			 CLASSTYPE_VBASECLASSES (lfi->type));
  else
    return binfo;
}

/* Within the scope of a template class, you can refer to the
   particular to the current specialization with the name of the
   template itself.  For example:
   
     template <typename T> struct S { S* sp; }

   Returns non-zero if DECL is such a declaration in a class TYPE.  */

static int
template_self_reference_p (type, decl)
     tree type;
     tree decl;
{
  return  (CLASSTYPE_USE_TEMPLATE (type)
	   && PRIMARY_TEMPLATE_P (CLASSTYPE_TI_TEMPLATE (type))
	   && TREE_CODE (decl) == TYPE_DECL
	   && DECL_ARTIFICIAL (decl)
	   && DECL_NAME (decl) == constructor_name (type));
}

/* DATA is really a struct lookup_field_info.  Look for a field with
   the name indicated there in BINFO.  If this function returns a
   non-NULL value it is the result of the lookup.  Called from
   lookup_field via breadth_first_search.  */

static tree
lookup_field_r (binfo, data)
     tree binfo;
     void *data;
{
  struct lookup_field_info *lfi = (struct lookup_field_info *) data;
  tree type = BINFO_TYPE (binfo);
  tree nval = NULL_TREE;
  int from_dep_base_p;

  /* First, look for a function.  There can't be a function and a data
     member with the same name, and if there's a function and a type
     with the same name, the type is hidden by the function.  */
  if (!lfi->want_type)
    {
      int idx = lookup_fnfields_here (type, lfi->name);
      if (idx >= 0)
	nval = TREE_VEC_ELT (CLASSTYPE_METHOD_VEC (type), idx);
    }

  if (!nval)
    /* Look for a data member or type.  */
    nval = lookup_field_1 (type, lfi->name);

  /* If there is no declaration with the indicated name in this type,
     then there's nothing to do.  */
  if (!nval)
    return NULL_TREE;

  /* If we're looking up a type (as with an elaborated type specifier)
     we ignore all non-types we find.  */
  if (lfi->want_type && TREE_CODE (nval) != TYPE_DECL)
    {
      nval = purpose_member (lfi->name, CLASSTYPE_TAGS (type));
      if (nval)
	nval = TYPE_MAIN_DECL (TREE_VALUE (nval));
      else 
	return NULL_TREE;
    }

  /* You must name a template base class with a template-id.  */
  if (!same_type_p (type, lfi->type) 
      && template_self_reference_p (type, nval))
    return NULL_TREE;

  from_dep_base_p = dependent_base_p (binfo);
  if (lfi->from_dep_base_p && !from_dep_base_p)
    {
      /* If the new declaration is not found via a dependent base, and
	 the old one was, then we must prefer the new one.  We weren't
	 really supposed to be able to find the old one, so we don't
	 want to be affected by a specialization.  Consider:

	   struct B { typedef int I; };
	   template <typename T> struct D1 : virtual public B {}; 
	   template <typename T> struct D :
	   public D1, virtual pubic B { I i; };

	 The `I' in `D<T>' is unambigousuly `B::I', regardless of how
	 D1 is specialized.  */
      lfi->from_dep_base_p = 0;
      lfi->rval = NULL_TREE;
      lfi->rval_binfo = NULL_TREE;
      lfi->ambiguous = NULL_TREE;
      lfi->errstr = 0;
    }
  else if (lfi->rval_binfo && !lfi->from_dep_base_p && from_dep_base_p)
    /* Similarly, if the old declaration was not found via a dependent
       base, and the new one is, ignore the new one.  */
    return NULL_TREE;

  /* If the lookup already found a match, and the new value doesn't
     hide the old one, we might have an ambiguity.  */
  if (lfi->rval_binfo && !hides (binfo, lfi->rval_binfo))
    {
      if (nval == lfi->rval && SHARED_MEMBER_P (nval))
	/* The two things are really the same.  */
	;
      else if (hides (lfi->rval_binfo, binfo))
	/* The previous value hides the new one.  */
	;
      else
	{
	  /* We have a real ambiguity.  We keep a chain of all the
	     candidates.  */
	  if (!lfi->ambiguous && lfi->rval)
	    {
	      /* This is the first time we noticed an ambiguity.  Add
		 what we previously thought was a reasonable candidate
		 to the list.  */
	      lfi->ambiguous = scratch_tree_cons (NULL_TREE, lfi->rval,
						  NULL_TREE);
	      TREE_TYPE (lfi->ambiguous) = error_mark_node;
	    }

	  /* Add the new value.  */
	  lfi->ambiguous = scratch_tree_cons (NULL_TREE, nval, 
					      lfi->ambiguous);
	  TREE_TYPE (lfi->ambiguous) = error_mark_node;
	  lfi->errstr = "request for member `%D' is ambiguous";
	}
    }
  else
    {
      /* If the thing we're looking for is a virtual base class, then
	 we know we've got what we want at this point; there's no way
	 to get an ambiguity.  */
      if (VBASE_NAME_P (lfi->name))
	{
	  lfi->rval = nval;
	  return nval;
	}

      if (from_dep_base_p && TREE_CODE (nval) != TYPE_DECL
	  /* We need to return a member template class so we can
	     define partial specializations.  Is there a better
	     way?  */
	  && !DECL_CLASS_TEMPLATE_P (nval))
	/* The thing we're looking for isn't a type, so the implicit
	   typename extension doesn't apply, so we just pretend we
	   didn't find anything.  */
	return NULL_TREE;

      lfi->rval = nval;
      lfi->from_dep_base_p = from_dep_base_p;
      lfi->rval_binfo = binfo;
    }

  return NULL_TREE;
}

/* Look for a memer named NAME in an inheritance lattice dominated by
   XBASETYPE.  PROTECT is 0 or two, we do not check access.  If it is
   1, we enforce accessibility.  If PROTECT is zero, then, for an
   ambiguous lookup, we return NULL.  If PROTECT is 1, we issue an
   error message.  If PROTECT is 2, we return a TREE_LIST whose
   TREEE_TYPE is error_mark_node and whose TREE_VALUEs are the list of
   ambiguous candidates.

   WANT_TYPE is 1 when we should only return TYPE_DECLs, if no
   TYPE_DECL can be found return NULL_TREE.  */

tree
lookup_member (xbasetype, name, protect, want_type)
     register tree xbasetype, name;
     int protect, want_type;
{
  tree rval, rval_binfo = NULL_TREE;
  tree type = NULL_TREE, basetype_path = NULL_TREE;
  struct lookup_field_info lfi;

  /* rval_binfo is the binfo associated with the found member, note,
     this can be set with useful information, even when rval is not
     set, because it must deal with ALL members, not just non-function
     members.  It is used for ambiguity checking and the hidden
     checks.  Whereas rval is only set if a proper (not hidden)
     non-function member is found.  */

  const char *errstr = 0;

  if (xbasetype == current_class_type && TYPE_BEING_DEFINED (xbasetype)
      && IDENTIFIER_CLASS_VALUE (name))
    {
      tree field = IDENTIFIER_CLASS_VALUE (name);
      if (TREE_CODE (field) != FUNCTION_DECL
	  && ! (want_type && TREE_CODE (field) != TYPE_DECL))
	/* We're in the scope of this class, and the value has already
	   been looked up.  Just return the cached value.  */
	return field;
    }

  if (TREE_CODE (xbasetype) == TREE_VEC)
    {
      type = BINFO_TYPE (xbasetype);
      basetype_path = xbasetype;
    }
  else if (IS_AGGR_TYPE_CODE (TREE_CODE (xbasetype)))
    {
      type = xbasetype;
      basetype_path = TYPE_BINFO (type);
      my_friendly_assert (BINFO_INHERITANCE_CHAIN (basetype_path) == NULL_TREE,
			  980827);
    }
  else
    my_friendly_abort (97);

  complete_type (type);

#ifdef GATHER_STATISTICS
  n_calls_lookup_field++;
#endif /* GATHER_STATISTICS */

  bzero ((PTR) &lfi, sizeof (lfi));
  lfi.type = type;
  lfi.name = name;
  lfi.want_type = want_type;
  bfs_walk (basetype_path, &lookup_field_r, &lookup_field_queue_p, &lfi);
  rval = lfi.rval;
  rval_binfo = lfi.rval_binfo;
  if (rval_binfo)
    type = BINFO_TYPE (rval_binfo);
  errstr = lfi.errstr;

  /* If we are not interested in ambiguities, don't report them;
     just return NULL_TREE.  */
  if (!protect && lfi.ambiguous)
    return NULL_TREE;
  
  if (protect == 2) 
    {
      if (lfi.ambiguous)
	return lfi.ambiguous;
      else
	protect = 0;
    }

  /* [class.access]

     In the case of overloaded function names, access control is
     applied to the function selected by overloaded resolution.  */
  if (rval && protect && !is_overloaded_fn (rval)
      && !IS_SIGNATURE_POINTER (DECL_REAL_CONTEXT (rval))
      && !IS_SIGNATURE_REFERENCE (DECL_REAL_CONTEXT (rval))
      && !enforce_access (xbasetype, rval))
    return error_mark_node;

  if (errstr && protect)
    {
      cp_error (errstr, name, type);
      if (lfi.ambiguous)
        print_candidates (lfi.ambiguous);
      rval = error_mark_node;
    }

  /* If the thing we found was found via the implicit typename
     extension, build the typename type.  */
  if (rval && lfi.from_dep_base_p && !DECL_CLASS_TEMPLATE_P (rval))
    rval = TYPE_STUB_DECL (build_typename_type (BINFO_TYPE (basetype_path),
						name, name,
						TREE_TYPE (rval)));

  if (rval && is_overloaded_fn (rval)) 
    {
      rval = scratch_tree_cons (basetype_path, rval, NULL_TREE);
      SET_BASELINK_P (rval);
    }

  return rval;
}

/* Like lookup_member, except that if we find a function member we
   return NULL_TREE.  */

tree
lookup_field (xbasetype, name, protect, want_type)
     register tree xbasetype, name;
     int protect, want_type;
{
  tree rval = lookup_member (xbasetype, name, protect, want_type);
  
  /* Ignore functions.  */
  if (rval && TREE_CODE (rval) == TREE_LIST)
    return NULL_TREE;

  return rval;
}

/* Like lookup_member, except that if we find a non-function member we
   return NULL_TREE.  */

tree
lookup_fnfields (xbasetype, name, protect)
     register tree xbasetype, name;
     int protect;
{
  tree rval = lookup_member (xbasetype, name, protect, /*want_type=*/0);

  /* Ignore non-functions.  */
  if (rval && TREE_CODE (rval) != TREE_LIST)
    return NULL_TREE;

  return rval;
}

/* TYPE is a class type. Return the index of the fields within
   the method vector with name NAME, or -1 is no such field exists.  */

int
lookup_fnfields_1 (type, name)
     tree type, name;
{
  register tree method_vec 
    = CLASS_TYPE_P (type) ? CLASSTYPE_METHOD_VEC (type) : NULL_TREE;

  if (method_vec != 0)
    {
      register tree *methods = &TREE_VEC_ELT (method_vec, 0);
      register tree *end = TREE_VEC_END (method_vec);

#ifdef GATHER_STATISTICS
      n_calls_lookup_fnfields_1++;
#endif /* GATHER_STATISTICS */

      /* Constructors are first...  */
      if (*methods && name == ctor_identifier)
	return 0;

      /* and destructors are second.  */
      if (*++methods && name == dtor_identifier)
	return 1;

      while (++methods != end && *methods)
	{
#ifdef GATHER_STATISTICS
	  n_outer_fields_searched++;
#endif /* GATHER_STATISTICS */
	  if (DECL_NAME (OVL_CURRENT (*methods)) == name)
	    break;
	}

      /* If we didn't find it, it might have been a template
	 conversion operator.  (Note that we don't look for this case
	 above so that we will always find specializations first.)  */
      if ((methods == end || !*methods)
	  && IDENTIFIER_TYPENAME_P (name)) 
	{
	  methods = &TREE_VEC_ELT (method_vec, 0) + 1;
	  
	  while (++methods != end && *methods)
	    {
	      tree method_name = DECL_NAME (OVL_CURRENT (*methods));

	      if (!IDENTIFIER_TYPENAME_P (method_name))
		{
		  /* Since all conversion operators come first, we know
		     there is no such operator.  */
		  methods = end;
		  break;
		}
	      else if (TREE_CODE (OVL_CURRENT (*methods)) == TEMPLATE_DECL)
		break;
	    }
	}

      if (methods != end && *methods)
	return methods - &TREE_VEC_ELT (method_vec, 0);
    }

  return -1;
}

/* Walk the class hierarchy dominated by TYPE.  FN is called for each
   type in the hierarchy, in a breadth-first preorder traversal.  .
   If it ever returns a non-NULL value, that value is immediately
   returned and the walk is terminated.  At each node FN, is passed a
   BINFO indicating the path from the curently visited base-class to
   TYPE.  The TREE_CHAINs of the BINFOs may be used for scratch space;
   they are otherwise unused.  Before each base-class is walked QFN is
   called.  If the value returned is non-zero, the base-class is
   walked; otherwise it is not.  If QFN is NULL, it is treated as a
   function which always returns 1.  Both FN and QFN are passed the
   DATA whenever they are called.  */

static tree
bfs_walk (binfo, fn, qfn, data)
     tree binfo;
     tree (*fn) PROTO((tree, void *));
     tree (*qfn) PROTO((tree, void *));
     void *data;
{
  size_t head;
  size_t tail;
  tree rval = NULL_TREE;
  /* An array of the base classes of BINFO.  These will be built up in
     breadth-first order, except where QFN prunes the search.  */
  varray_type bfs_bases;

  /* Start with enough room for ten base classes.  That will be enough
     for most hierarchies.  */
  VARRAY_TREE_INIT (bfs_bases, 10, "search_stack");

  /* Put the first type into the stack.  */
  VARRAY_TREE (bfs_bases, 0) = binfo;
  tail = 1;

  for (head = 0; head < tail; ++head)
    {
      int i;
      int n_baselinks;
      tree binfos;

      /* Pull the next type out of the queue.  */
      binfo = VARRAY_TREE (bfs_bases, head);

      /* If this is the one we're looking for, we're done.  */
      rval = (*fn) (binfo, data);
      if (rval)
	break;

      /* Queue up the base types.  */
      binfos = BINFO_BASETYPES (binfo);
      n_baselinks = binfos ? TREE_VEC_LENGTH (binfos): 0;
      for (i = 0; i < n_baselinks; i++)
	{
	  tree base_binfo = TREE_VEC_ELT (binfos, i);

	  if (qfn)
	    base_binfo = (*qfn) (base_binfo, data);

	  if (base_binfo)
	    {
	      if (tail == VARRAY_SIZE (bfs_bases))
		VARRAY_GROW (bfs_bases, 2 * VARRAY_SIZE (bfs_bases));
	      VARRAY_TREE (bfs_bases, tail) = base_binfo;
	      ++tail;
	    }
	}
    }

  /* Clean up.  */
  VARRAY_FREE (bfs_bases);

  return rval;
}

/* Exactly like bfs_walk, except that a depth-first traversal is
   performed, and PREFN is called in preorder, while POSTFN is called
   in postorder.  */

static tree
dfs_walk_real (binfo, prefn, postfn, qfn, data)
     tree binfo;
     tree (*prefn) PROTO((tree, void *));
     tree (*postfn) PROTO((tree, void *));
     tree (*qfn) PROTO((tree, void *));
     void *data;
{
  int i;
  int n_baselinks;
  tree binfos;
  tree rval = NULL_TREE;

  /* Call the pre-order walking function.  */
  if (prefn)
    {
      rval = (*prefn) (binfo, data);
      if (rval)
	return rval;
    }

  /* Process the basetypes.  */
  binfos = BINFO_BASETYPES (binfo);
  n_baselinks = binfos ? TREE_VEC_LENGTH (binfos): 0;
  for (i = 0; i < n_baselinks; i++)
    {
      tree base_binfo = TREE_VEC_ELT (binfos, i);
      
      if (qfn)
	base_binfo = (*qfn) (base_binfo, data);

      if (base_binfo)
	{
	  rval = dfs_walk_real (base_binfo, prefn, postfn, qfn, data);
	  if (rval)
	    return rval;
	}
    }

  /* Call the post-order walking function.  */
  if (postfn)
    rval = (*postfn) (binfo, data);
  
  return rval;
}

/* Exactly like bfs_walk, except that a depth-first post-order traversal is
   performed.  */

tree
dfs_walk (binfo, fn, qfn, data)
     tree binfo;
     tree (*fn) PROTO((tree, void *));
     tree (*qfn) PROTO((tree, void *));
     void *data;
{
  return dfs_walk_real (binfo, 0, fn, qfn, data);
}

struct gvnt_info 
{
  /* The name of the function we are looking for.  */
  tree name;
  /* The overloaded functions we have found.  */
  tree fields;
};

/* Called from get_virtuals_named_this via bfs_walk.  */

static tree
get_virtuals_named_this_r (binfo, data)
     tree binfo;
     void *data;
{
  struct gvnt_info *gvnti = (struct gvnt_info *) data;
  tree type = BINFO_TYPE (binfo);
  int idx;

  idx = lookup_fnfields_here (BINFO_TYPE (binfo), gvnti->name);
  if (idx >= 0)
    gvnti->fields
      = scratch_tree_cons (binfo, 
			   TREE_VEC_ELT (CLASSTYPE_METHOD_VEC (type),
					 idx),
			   gvnti->fields);

  return NULL_TREE;
}

/* Return the virtual functions with the indicated NAME in the type
   indicated by BINFO.  The result is a TREE_LIST whose TREE_PURPOSE
   indicates the base class from which the TREE_VALUE (an OVERLOAD or
   just a FUNCTION_DECL) originated.  */

static tree
get_virtuals_named_this (binfo, name)
     tree binfo;
     tree name;
{
  struct gvnt_info gvnti;
  tree fields;

  gvnti.name = name;
  gvnti.fields = NULL_TREE;

  bfs_walk (binfo, get_virtuals_named_this_r, 0, &gvnti);

  /* Get to the function decls, and return the first virtual function
     with this name, if there is one.  */
  for (fields = gvnti.fields; fields; fields = next_baselink (fields))
    {
      tree fndecl;

      for (fndecl = TREE_VALUE (fields); fndecl; fndecl = OVL_NEXT (fndecl))
	if (DECL_VINDEX (OVL_CURRENT (fndecl)))
	  return fields;
    }
  return NULL_TREE;
}

static tree
get_virtual_destructor (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  tree type = BINFO_TYPE (binfo);
  if (TYPE_HAS_DESTRUCTOR (type)
      && DECL_VINDEX (TREE_VEC_ELT (CLASSTYPE_METHOD_VEC (type), 1)))
    return TREE_VEC_ELT (CLASSTYPE_METHOD_VEC (type), 1);
  return 0;
}

static tree
tree_has_any_destructor_p (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  tree type = BINFO_TYPE (binfo);
  return TYPE_NEEDS_DESTRUCTOR (type) ? binfo : NULL_TREE;
}

/* Returns > 0 if a function with type DRETTYPE overriding a function
   with type BRETTYPE is covariant, as defined in [class.virtual].

   Returns 1 if trivial covariance, 2 if non-trivial (requiring runtime
   adjustment), or -1 if pedantically invalid covariance.  */

static int
covariant_return_p (brettype, drettype)
     tree brettype, drettype;
{
  tree binfo;

  if (TREE_CODE (brettype) == FUNCTION_DECL
      || TREE_CODE (brettype) == THUNK_DECL)
    {
      brettype = TREE_TYPE (TREE_TYPE (brettype));
      drettype = TREE_TYPE (TREE_TYPE (drettype));
    }
  else if (TREE_CODE (brettype) == METHOD_TYPE)
    {
      brettype = TREE_TYPE (brettype);
      drettype = TREE_TYPE (drettype);
    }

  if (same_type_p (brettype, drettype))
    return 0;

  if (! (TREE_CODE (brettype) == TREE_CODE (drettype)
	 && (TREE_CODE (brettype) == POINTER_TYPE
	     || TREE_CODE (brettype) == REFERENCE_TYPE)
	 && TYPE_QUALS (brettype) == TYPE_QUALS (drettype)))
    return 0;

  if (! can_convert (brettype, drettype))
    return 0;

  brettype = TREE_TYPE (brettype);
  drettype = TREE_TYPE (drettype);

  /* If not pedantic, allow any standard pointer conversion.  */
  if (! IS_AGGR_TYPE (drettype) || ! IS_AGGR_TYPE (brettype))
    return -1;

  binfo = get_binfo (brettype, drettype, 1);

  /* If we get an error_mark_node from get_binfo, it already complained,
     so let's just succeed.  */
  if (binfo == error_mark_node)
    return 1;

  if (! BINFO_OFFSET_ZEROP (binfo) || TREE_VIA_VIRTUAL (binfo))
    return 2;
  return 1;
}

/* Given a class type TYPE, and a function decl FNDECL, look for a
   virtual function in TYPE's hierarchy which FNDECL could match as a
   virtual function.  It doesn't matter which one we find.

   DTORP is nonzero if we are looking for a destructor.  Destructors
   need special treatment because they do not match by name.  */

tree
get_matching_virtual (binfo, fndecl, dtorp)
     tree binfo, fndecl;
     int dtorp;
{
  tree tmp = NULL_TREE;
  int i;

  if (TREE_CODE (fndecl) == TEMPLATE_DECL)
    /* In [temp.mem] we have:

         A specialization of a member function template does not
         override a virtual function from a base class.  */
    return NULL_TREE;

  /* Breadth first search routines start searching basetypes
     of TYPE, so we must perform first ply of search here.  */
  if (dtorp)
    return bfs_walk (binfo, get_virtual_destructor,
		     tree_has_any_destructor_p, 0);
  else
    {
      tree drettype, dtypes, btypes, instptr_type;
      tree basetype = DECL_CLASS_CONTEXT (fndecl);
      tree baselink, best = NULL_TREE;
      tree name = DECL_ASSEMBLER_NAME (fndecl);
      tree declarator = DECL_NAME (fndecl);
      if (IDENTIFIER_VIRTUAL_P (declarator) == 0)
	return NULL_TREE;

      baselink = get_virtuals_named_this (binfo, declarator);
      if (baselink == NULL_TREE)
	return NULL_TREE;

      drettype = TREE_TYPE (TREE_TYPE (fndecl));
      dtypes = TYPE_ARG_TYPES (TREE_TYPE (fndecl));
      if (DECL_STATIC_FUNCTION_P (fndecl))
	instptr_type = NULL_TREE;
      else
	instptr_type = TREE_TYPE (TREE_VALUE (dtypes));

      for (; baselink; baselink = next_baselink (baselink))
	{
	  tree tmps;
	  for (tmps = TREE_VALUE (baselink); tmps; tmps = OVL_NEXT (tmps))
	    {
	      tmp = OVL_CURRENT (tmps);
	      if (! DECL_VINDEX (tmp))
		continue;

	      btypes = TYPE_ARG_TYPES (TREE_TYPE (tmp));
	      if (instptr_type == NULL_TREE)
		{
		  if (compparms (TREE_CHAIN (btypes), dtypes))
		    /* Caller knows to give error in this case.  */
		    return tmp;
		  return NULL_TREE;
		}

	      if (/* The first parameter is the `this' parameter,
		     which has POINTER_TYPE, and we can therefore
		     safely use TYPE_QUALS, rather than
		     CP_TYPE_QUALS.  */
		  (TYPE_QUALS (TREE_TYPE (TREE_VALUE (btypes)))
		   == TYPE_QUALS (instptr_type))
		  && compparms (TREE_CHAIN (btypes), TREE_CHAIN (dtypes)))
		{
		  tree brettype = TREE_TYPE (TREE_TYPE (tmp));
		  if (same_type_p (brettype, drettype))
		    /* OK */;
		  else if ((i = covariant_return_p (brettype, drettype)))
		    {
		      if (i == 2)
			sorry ("adjusting pointers for covariant returns");

		      if (pedantic && i == -1)
			{
			  cp_pedwarn_at ("invalid covariant return type for `%#D' (must be pointer or reference to class)", fndecl);
			  cp_pedwarn_at ("  overriding `%#D'", tmp);
			}
		    }
		  else if (IS_AGGR_TYPE_2 (brettype, drettype)
			   && same_or_base_type_p (brettype, drettype))
		    {
		      error ("invalid covariant return type (must use pointer or reference)");
		      cp_error_at ("  overriding `%#D'", tmp);
		      cp_error_at ("  with `%#D'", fndecl);
		    }
		  else if (IDENTIFIER_ERROR_LOCUS (name) == NULL_TREE)
		    {
		      cp_error_at ("conflicting return type specified for virtual function `%#D'", fndecl);
		      cp_error_at ("  overriding definition as `%#D'", tmp);
		      SET_IDENTIFIER_ERROR_LOCUS (name, basetype);
		    }

		  /* FNDECL overrides this function.  We continue to
		     check all the other functions in order to catch
		     errors; it might be that in some other baseclass
		     a virtual function was declared with the same
		     parameter types, but a different return type.  */
		  best = tmp;
		}
	    }
	}

      return best;
    }
}

/* Return the list of virtual functions which are abstract in type
   TYPE that come from non virtual base classes.  See
   expand_direct_vtbls_init for the style of search we do.  */

static tree
get_abstract_virtuals_1 (binfo, do_self, abstract_virtuals)
     tree binfo;
     int do_self;
     tree abstract_virtuals;
{
  tree binfos = BINFO_BASETYPES (binfo);
  int i, n_baselinks = binfos ? TREE_VEC_LENGTH (binfos) : 0;

  for (i = 0; i < n_baselinks; i++)
    {
      tree base_binfo = TREE_VEC_ELT (binfos, i);
      int is_not_base_vtable
	= i != CLASSTYPE_VFIELD_PARENT (BINFO_TYPE (binfo));
      if (! TREE_VIA_VIRTUAL (base_binfo))
	abstract_virtuals
	  = get_abstract_virtuals_1 (base_binfo, is_not_base_vtable,
				     abstract_virtuals);
    }
  /* Should we use something besides CLASSTYPE_VFIELDS? */
  if (do_self && CLASSTYPE_VFIELDS (BINFO_TYPE (binfo)))
    {
      tree virtuals = BINFO_VIRTUALS (binfo);

      skip_rtti_stuff (&virtuals, BINFO_TYPE (binfo));

      while (virtuals)
	{
	  tree base_pfn = FNADDR_FROM_VTABLE_ENTRY (TREE_VALUE (virtuals));
	  tree base_fndecl = TREE_OPERAND (base_pfn, 0);
	  if (DECL_ABSTRACT_VIRTUAL_P (base_fndecl))
	    abstract_virtuals = tree_cons (NULL_TREE, base_fndecl, abstract_virtuals);
	  virtuals = TREE_CHAIN (virtuals);
	}
    }
  return abstract_virtuals;
}

/* Return the list of virtual functions which are abstract in type TYPE.
   This information is cached, and so must be built on a
   non-temporary obstack.  */

tree
get_abstract_virtuals (type)
     tree type;
{
  tree vbases;
  tree abstract_virtuals = NULL;

  /* First get all from non-virtual bases.  */
  abstract_virtuals
    = get_abstract_virtuals_1 (TYPE_BINFO (type), 1, abstract_virtuals);
					       
  for (vbases = CLASSTYPE_VBASECLASSES (type); vbases; vbases = TREE_CHAIN (vbases))
    {
      tree virtuals = BINFO_VIRTUALS (vbases);

      skip_rtti_stuff (&virtuals, type);

      while (virtuals)
	{
	  tree base_pfn = FNADDR_FROM_VTABLE_ENTRY (TREE_VALUE (virtuals));
	  tree base_fndecl = TREE_OPERAND (base_pfn, 0);
	  if (DECL_NEEDS_FINAL_OVERRIDER_P (base_fndecl))
	    cp_error ("`%#D' needs a final overrider", base_fndecl);
	  else if (DECL_ABSTRACT_VIRTUAL_P (base_fndecl))
	    abstract_virtuals = tree_cons (NULL_TREE, base_fndecl, abstract_virtuals);
	  virtuals = TREE_CHAIN (virtuals);
	}
    }
  return nreverse (abstract_virtuals);
}

static tree
next_baselink (baselink)
     tree baselink;
{
  tree tmp = TREE_TYPE (baselink);
  baselink = TREE_CHAIN (baselink);
  while (tmp)
    {
      /* @@ does not yet add previous base types.  */
      baselink = tree_cons (TREE_PURPOSE (tmp), TREE_VALUE (tmp),
			    baselink);
      TREE_TYPE (baselink) = TREE_TYPE (tmp);
      tmp = TREE_CHAIN (tmp);
    }
  return baselink;
}

/* DEPTH-FIRST SEARCH ROUTINES.  */

/* This routine converts a pointer to be a pointer of an immediate
   base class.  The normal convert_pointer_to routine would diagnose
   the conversion as ambiguous, under MI code that has the base class
   as an ambiguous base class.  */

static tree
convert_pointer_to_single_level (to_type, expr)
     tree to_type, expr;
{
  tree derived;
  tree binfo_of_derived;
  int i;

  derived = TREE_TYPE (TREE_TYPE (expr));
  binfo_of_derived = TYPE_BINFO (derived);
  my_friendly_assert (BINFO_INHERITANCE_CHAIN (binfo_of_derived) == NULL_TREE,
		      980827);
  for (i = CLASSTYPE_N_BASECLASSES (derived) - 1; i >= 0; --i)
    {
      tree binfo = BINFO_BASETYPE (binfo_of_derived, i);
      my_friendly_assert (BINFO_INHERITANCE_CHAIN (binfo) == binfo_of_derived,
			  980827);
      if (same_type_p (BINFO_TYPE (binfo), to_type))
	return build_vbase_path (PLUS_EXPR, 
				 build_pointer_type (to_type), 
				 expr, binfo, 1);
    }

  my_friendly_abort (19990607);

  /* NOTREACHED */
  return NULL_TREE;
}

tree markedp (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{ 
  return BINFO_MARKED (binfo) ? binfo : NULL_TREE; 
}

static tree
unmarkedp (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  return !BINFO_MARKED (binfo) ? binfo : NULL_TREE;
}

static tree
marked_vtable_pathp (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{ 
  return BINFO_VTABLE_PATH_MARKED (binfo) ? binfo : NULL_TREE; 
}

static tree
unmarked_vtable_pathp (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{ 
  return !BINFO_VTABLE_PATH_MARKED (binfo) ? binfo : NULL_TREE; 
}

static tree 
marked_new_vtablep (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  return BINFO_NEW_VTABLE_MARKED (binfo) ? binfo : NULL_TREE; 
}

static tree
unmarked_new_vtablep (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{ 
  return !BINFO_NEW_VTABLE_MARKED (binfo) ? binfo : NULL_TREE; 
}

static tree
marked_pushdecls_p (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  return (CLASS_TYPE_P (BINFO_TYPE (binfo))
	  && BINFO_PUSHDECLS_MARKED (binfo)) ? binfo : NULL_TREE; 
}

static tree
unmarked_pushdecls_p (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{ 
  return (CLASS_TYPE_P (BINFO_TYPE (binfo))
	  && !BINFO_PUSHDECLS_MARKED (binfo)) ? binfo : NULL_TREE;
}

#if 0
static int dfs_search_slot_nonempty_p (binfo) tree binfo;
{ return CLASSTYPE_SEARCH_SLOT (BINFO_TYPE (binfo)) != 0; }
#endif

static tree 
dfs_debug_unmarkedp (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{ 
  return (!CLASSTYPE_DEBUG_REQUESTED (BINFO_TYPE (binfo)) 
	  ? binfo : NULL_TREE);
}

/* The worker functions for `dfs_walk'.  These do not need to
   test anything (vis a vis marking) if they are paired with
   a predicate function (above).  */

#if 0
static void
dfs_mark (binfo) tree binfo;
{ SET_BINFO_MARKED (binfo); }
#endif

tree
dfs_unmark (binfo, data) 
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{ 
  CLEAR_BINFO_MARKED (binfo); 
  return NULL_TREE;
}

#if 0
static void
dfs_mark_vtable_path (binfo) tree binfo;
{ SET_BINFO_VTABLE_PATH_MARKED (binfo); }

static void
dfs_unmark_vtable_path (binfo) tree binfo;
{ CLEAR_BINFO_VTABLE_PATH_MARKED (binfo); }

static void
dfs_mark_new_vtable (binfo) tree binfo;
{ SET_BINFO_NEW_VTABLE_MARKED (binfo); }

static void
dfs_unmark_new_vtable (binfo) tree binfo;
{ CLEAR_BINFO_NEW_VTABLE_MARKED (binfo); }

static void
dfs_clear_search_slot (binfo) tree binfo;
{ CLASSTYPE_SEARCH_SLOT (BINFO_TYPE (binfo)) = 0; }
#endif

static tree
dfs_debug_mark (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  tree t = BINFO_TYPE (binfo);

  /* Use heuristic that if there are virtual functions,
     ignore until we see a non-inline virtual function.  */
  tree methods = CLASSTYPE_METHOD_VEC (t);

  CLASSTYPE_DEBUG_REQUESTED (t) = 1;

  if (methods == 0)
    return NULL_TREE;

  /* If interface info is known, either we've already emitted the debug
     info or we don't need to.  */
  if (CLASSTYPE_INTERFACE_KNOWN (t))
    return NULL_TREE;

  /* If debug info is requested from this context for this type, supply it.
     If debug info is requested from another context for this type,
     see if some third context can supply it.  */
  if (current_function_decl == NULL_TREE
      || DECL_CLASS_CONTEXT (current_function_decl) != t)
    {
      if (TREE_VEC_ELT (methods, 1))
	methods = TREE_VEC_ELT (methods, 1);
      else if (TREE_VEC_ELT (methods, 0))
	methods = TREE_VEC_ELT (methods, 0);
      else
	methods = TREE_VEC_ELT (methods, 2);
      methods = OVL_CURRENT (methods);
      while (methods)
	{
	  if (DECL_VINDEX (methods)
	      && DECL_THIS_INLINE (methods) == 0
	      && DECL_ABSTRACT_VIRTUAL_P (methods) == 0)
	    {
	      /* Somebody, somewhere is going to have to define this
		 virtual function.  When they do, they will provide
		 the debugging info.  */
	      return NULL_TREE;
	    }
	  methods = TREE_CHAIN (methods);
	}
    }
  /* We cannot rely on some alien method to solve our problems,
     so we must write out the debug info ourselves.  */
  TYPE_DECL_SUPPRESS_DEBUG (TYPE_NAME (t)) = 0;
  rest_of_type_compilation (t, toplevel_bindings_p ());

  return NULL_TREE;
}

struct vbase_info 
{
  tree decl_ptr;
  tree inits;
  tree vbase_types;
};

/*  Attach to the type of the virtual base class, the pointer to the
    virtual base class.  */

static tree
dfs_find_vbases (binfo, data)
     tree binfo;
     void *data;
{
  struct vbase_info *vi = (struct vbase_info *) data;
  tree binfos = BINFO_BASETYPES (binfo);
  int i, n_baselinks = binfos ? TREE_VEC_LENGTH (binfos) : 0;

  for (i = n_baselinks-1; i >= 0; i--)
    {
      tree base_binfo = TREE_VEC_ELT (binfos, i);

      if (TREE_VIA_VIRTUAL (base_binfo)
	  && CLASSTYPE_SEARCH_SLOT (BINFO_TYPE (base_binfo)) == 0)
	{
	  tree vbase = BINFO_TYPE (base_binfo);
	  tree binfo = binfo_member (vbase, vi->vbase_types);

	  CLASSTYPE_SEARCH_SLOT (vbase)
	    = build (PLUS_EXPR, build_pointer_type (vbase),
		     vi->decl_ptr, BINFO_OFFSET (binfo));
	}
    }
  SET_BINFO_VTABLE_PATH_MARKED (binfo);
  SET_BINFO_NEW_VTABLE_MARKED (binfo);

  return NULL_TREE;
}

static tree
dfs_init_vbase_pointers (binfo, data)
     tree binfo;
     void *data;
{
  struct vbase_info *vi = (struct vbase_info *) data;
  tree type = BINFO_TYPE (binfo);
  tree fields = TYPE_FIELDS (type);
  tree this_vbase_ptr;

  CLEAR_BINFO_VTABLE_PATH_MARKED (binfo);

#if 0
  /* See finish_struct_1 for when we can enable this.  */
  /* If we have a vtable pointer first, skip it.  */
  if (VFIELD_NAME_P (DECL_NAME (fields)))
    fields = TREE_CHAIN (fields);
#endif

  if (BINFO_INHERITANCE_CHAIN (binfo))
    {
      this_vbase_ptr = TREE_CHAIN (BINFO_INHERITANCE_CHAIN (binfo));
      if (TREE_VIA_VIRTUAL (binfo))
	this_vbase_ptr = CLASSTYPE_SEARCH_SLOT (type);
      else
	this_vbase_ptr = convert_pointer_to_single_level (type,
							  this_vbase_ptr); 
      TREE_CHAIN (binfo) = this_vbase_ptr;
    }
  else
    this_vbase_ptr = TREE_CHAIN (binfo);

  if (fields == NULL_TREE
      || DECL_NAME (fields) == NULL_TREE
      || ! VBASE_NAME_P (DECL_NAME (fields)))
    return NULL_TREE;

  if (build_pointer_type (type) 
      != TYPE_MAIN_VARIANT (TREE_TYPE (this_vbase_ptr)))
    my_friendly_abort (125);

  while (fields && DECL_NAME (fields) && VBASE_NAME_P (DECL_NAME (fields)))
    {
      tree ref = build (COMPONENT_REF, TREE_TYPE (fields),
			build_indirect_ref (this_vbase_ptr, NULL_PTR), fields);
      tree init = CLASSTYPE_SEARCH_SLOT (TREE_TYPE (TREE_TYPE (fields)));
      vi->inits = tree_cons (binfo_member (TREE_TYPE (TREE_TYPE (fields)),
					   vi->vbase_types),
			     build_modify_expr (ref, NOP_EXPR, init),
			     vi->inits);
      fields = TREE_CHAIN (fields);
    }
  
  return NULL_TREE;
}

/* Sometimes this needs to clear both VTABLE_PATH and NEW_VTABLE.  Other
   times, just NEW_VTABLE, but optimizer should make both with equal
   efficiency (though it does not currently).  */

static tree
dfs_clear_vbase_slots (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  tree type = BINFO_TYPE (binfo);
  CLASSTYPE_SEARCH_SLOT (type) = 0;
  CLEAR_BINFO_VTABLE_PATH_MARKED (binfo);
  CLEAR_BINFO_NEW_VTABLE_MARKED (binfo);
  return NULL_TREE;
}

tree
init_vbase_pointers (type, decl_ptr)
     tree type;
     tree decl_ptr;
{
  if (TYPE_USES_VIRTUAL_BASECLASSES (type))
    {
      struct vbase_info vi;
      int old_flag = flag_this_is_variable;
      tree binfo = TYPE_BINFO (type);
      flag_this_is_variable = -2;

      /* Find all the virtual base classes, marking them for later
	 initialization.  */
      vi.decl_ptr = decl_ptr;
      vi.vbase_types = CLASSTYPE_VBASECLASSES (type);
      vi.inits = NULL_TREE;

      dfs_walk (binfo, dfs_find_vbases, unmarked_vtable_pathp, &vi);

      /* Build up a list of the initializers.  */
      TREE_CHAIN (binfo) = decl_ptr;
      dfs_walk_real (binfo, 
		     dfs_init_vbase_pointers, 0,
		     marked_vtable_pathp,
		     &vi);

      dfs_walk (binfo, dfs_clear_vbase_slots, marked_new_vtablep, 0);
      flag_this_is_variable = old_flag;
      return vi.inits;
    }
  return 0;
}

/* get the virtual context (the vbase that directly contains the
   DECL_CLASS_CONTEXT of the FNDECL) that the given FNDECL is declared in,
   or NULL_TREE if there is none.

   FNDECL must come from a virtual table from a virtual base to ensure that
   there is only one possible DECL_CLASS_CONTEXT.

   We know that if there is more than one place (binfo) the fndecl that the
   declared, they all refer to the same binfo.  See get_class_offset_1 for
   the check that ensures this.  */

static tree
virtual_context (fndecl, t, vbase)
     tree fndecl, t, vbase;
{
  tree path;
  if (get_base_distance (DECL_CLASS_CONTEXT (fndecl), t, 0, &path) < 0)
    {
      /* DECL_CLASS_CONTEXT can be ambiguous in t.  */
      if (get_base_distance (DECL_CLASS_CONTEXT (fndecl), vbase, 0, &path) >= 0)
	{
	  while (path)
	    {
	      /* Not sure if checking path == vbase is necessary here, but just in
		 case it is.  */
	      if (TREE_VIA_VIRTUAL (path) || path == vbase)
		return binfo_member (BINFO_TYPE (path), CLASSTYPE_VBASECLASSES (t));
	      path = BINFO_INHERITANCE_CHAIN (path);
	    }
	}
      /* This shouldn't happen, I don't want errors! */
      warning ("recoverable compiler error, fixups for virtual function");
      return vbase;
    }
  while (path)
    {
      if (TREE_VIA_VIRTUAL (path))
	return binfo_member (BINFO_TYPE (path), CLASSTYPE_VBASECLASSES (t));
      path = BINFO_INHERITANCE_CHAIN (path);
    }
  return 0;
}

/* Fixups upcast offsets for one vtable.
   Entries may stay within the VBASE given, or
   they may upcast into a direct base, or
   they may upcast into a different vbase.

   We only need to do fixups in case 2 and 3.  In case 2, we add in
   the virtual base offset to effect an upcast, in case 3, we add in
   the virtual base offset to effect an upcast, then subtract out the
   offset for the other virtual base, to effect a downcast into it.

   This routine mirrors fixup_vtable_deltas in functionality, though
   this one is runtime based, and the other is compile time based.
   Conceivably that routine could be removed entirely, and all fixups
   done at runtime.

   VBASE_OFFSETS is an association list of virtual bases that contains
   offset information for the virtual bases, so the offsets are only
   calculated once.  The offsets are computed by where we think the
   vbase should be (as noted by the CLASSTYPE_SEARCH_SLOT) minus where
   the vbase really is.  */

static void
expand_upcast_fixups (binfo, addr, orig_addr, vbase, vbase_addr, t,
		      vbase_offsets)
     tree binfo, addr, orig_addr, vbase, vbase_addr, t, *vbase_offsets;
{
  tree virtuals = BINFO_VIRTUALS (binfo);
  tree vc;
  tree delta;
  unsigned HOST_WIDE_INT n;
  
  delta = purpose_member (vbase, *vbase_offsets);
  if (! delta)
    {
      delta = CLASSTYPE_SEARCH_SLOT (BINFO_TYPE (vbase));
      delta = build (MINUS_EXPR, ptrdiff_type_node, delta, vbase_addr);
      delta = save_expr (delta);
      delta = tree_cons (vbase, delta, *vbase_offsets);
      *vbase_offsets = delta;
    }

  n = skip_rtti_stuff (&virtuals, t);

  while (virtuals)
    {
      tree current_fndecl = TREE_VALUE (virtuals);
      current_fndecl = FNADDR_FROM_VTABLE_ENTRY (current_fndecl);
      current_fndecl = TREE_OPERAND (current_fndecl, 0);
      if (current_fndecl
	  && current_fndecl != abort_fndecl
	  && (vc=virtual_context (current_fndecl, t, vbase)) != vbase)
	{
	  /* This may in fact need a runtime fixup.  */
	  tree idx = build_int_2 (n, 0);
	  tree vtbl = BINFO_VTABLE (binfo);
	  tree nvtbl = lookup_name (DECL_NAME (vtbl), 0);
	  tree aref, ref, naref;
	  tree old_delta, new_delta;
	  tree init;

	  if (nvtbl == NULL_TREE
	      || nvtbl == IDENTIFIER_GLOBAL_VALUE (DECL_NAME (vtbl)))
	    {
	      /* Dup it if it isn't in local scope yet.  */
	      nvtbl = build_decl
		(VAR_DECL, DECL_NAME (vtbl),
		 TYPE_MAIN_VARIANT (TREE_TYPE (vtbl)));
	      DECL_ALIGN (nvtbl) = MAX (TYPE_ALIGN (double_type_node),
					DECL_ALIGN (nvtbl));
	      TREE_READONLY (nvtbl) = 0;
	      DECL_ARTIFICIAL (nvtbl) = 1;
	      nvtbl = pushdecl (nvtbl);
	      init = NULL_TREE;
	      cp_finish_decl (nvtbl, init, NULL_TREE, 0,
			      LOOKUP_ONLYCONVERTING);

	      /* We don't set DECL_VIRTUAL_P and DECL_CONTEXT on nvtbl
		 because they wouldn't be useful; everything that wants to
		 look at the vtable will look at the decl for the normal
		 vtable.  Setting DECL_CONTEXT also screws up
		 decl_function_context.  */

	      init = build (MODIFY_EXPR, TREE_TYPE (nvtbl),
			    nvtbl, vtbl);
	      TREE_SIDE_EFFECTS (init) = 1;
	      expand_expr_stmt (init);
	      /* Update the vtable pointers as necessary.  */
	      ref = build_vfield_ref
		(build_indirect_ref (addr, NULL_PTR),
		 DECL_CONTEXT (CLASSTYPE_VFIELD (BINFO_TYPE (binfo))));
	      expand_expr_stmt
		(build_modify_expr (ref, NOP_EXPR, nvtbl));
	    }
	  assemble_external (vtbl);
	  aref = build_array_ref (vtbl, idx);
	  naref = build_array_ref (nvtbl, idx);
	  old_delta = build_component_ref (aref, delta_identifier,
					   NULL_TREE, 0);
	  new_delta = build_component_ref (naref, delta_identifier,
					   NULL_TREE, 0);

	  /* This is a upcast, so we have to add the offset for the
	     virtual base.  */
	  old_delta = build_binary_op (PLUS_EXPR, old_delta,
				       TREE_VALUE (delta));
	  if (vc)
	    {
	      /* If this is set, we need to subtract out the delta
		 adjustments for the other virtual base that we
		 downcast into.  */
	      tree vc_delta = purpose_member (vc, *vbase_offsets);
	      if (! vc_delta)
		{
		  tree vc_addr = convert_pointer_to_real (vc, orig_addr);
		  vc_delta = CLASSTYPE_SEARCH_SLOT (BINFO_TYPE (vc));
		  vc_delta = build (MINUS_EXPR, ptrdiff_type_node,
				    vc_delta, vc_addr);
		  vc_delta = save_expr (vc_delta);
		  *vbase_offsets = tree_cons (vc, vc_delta, *vbase_offsets);
		}
	      else
		vc_delta = TREE_VALUE (vc_delta);
   
	      /* This is a downcast, so we have to subtract the offset
		 for the virtual base.  */
	      old_delta = build_binary_op (MINUS_EXPR, old_delta, vc_delta);
	    }

	  TREE_READONLY (new_delta) = 0;
	  TREE_TYPE (new_delta) = 
	    cp_build_qualified_type (TREE_TYPE (new_delta),
				     CP_TYPE_QUALS (TREE_TYPE (new_delta))
				     & ~TYPE_QUAL_CONST);
	  expand_expr_stmt (build_modify_expr (new_delta, NOP_EXPR,
					       old_delta));
	}
      ++n;
      virtuals = TREE_CHAIN (virtuals);
    }
}

/* Fixup upcast offsets for all direct vtables.  Patterned after
   expand_direct_vtbls_init.  */

static void
fixup_virtual_upcast_offsets (real_binfo, binfo, init_self, can_elide, addr, orig_addr, type, vbase, vbase_offsets)
     tree real_binfo, binfo;
     int init_self, can_elide;
     tree addr, orig_addr, type, vbase, *vbase_offsets;
{
  tree real_binfos = BINFO_BASETYPES (real_binfo);
  tree binfos = BINFO_BASETYPES (binfo);
  int i, n_baselinks = real_binfos ? TREE_VEC_LENGTH (real_binfos) : 0;

  for (i = 0; i < n_baselinks; i++)
    {
      tree real_base_binfo = TREE_VEC_ELT (real_binfos, i);
      tree base_binfo = TREE_VEC_ELT (binfos, i);
      int is_not_base_vtable
	= i != CLASSTYPE_VFIELD_PARENT (BINFO_TYPE (real_binfo));
      if (! TREE_VIA_VIRTUAL (real_base_binfo))
	fixup_virtual_upcast_offsets (real_base_binfo, base_binfo,
				      is_not_base_vtable, can_elide, addr,
				      orig_addr, type, vbase, vbase_offsets);
    }
#if 0
  /* Before turning this on, make sure it is correct.  */
  if (can_elide && ! BINFO_MODIFIED (binfo))
    return;
#endif
  /* Should we use something besides CLASSTYPE_VFIELDS? */
  if (init_self && CLASSTYPE_VFIELDS (BINFO_TYPE (real_binfo)))
    {
      tree new_addr = convert_pointer_to_real (binfo, addr);
      expand_upcast_fixups (real_binfo, new_addr, orig_addr, vbase, addr,
			    type, vbase_offsets);
    }
}

/* Build a COMPOUND_EXPR which when expanded will generate the code
   needed to initialize all the virtual function table slots of all
   the virtual baseclasses.  MAIN_BINFO is the binfo which determines
   the virtual baseclasses to use; TYPE is the type of the object to
   which the initialization applies.  TRUE_EXP is the true object we
   are initializing, and DECL_PTR is the pointer to the sub-object we
   are initializing.

   When USE_COMPUTED_OFFSETS is non-zero, we can assume that the
   object was laid out by a top-level constructor and the computed
   offsets are valid to store vtables.  When zero, we must store new
   vtables through virtual baseclass pointers.  */

void
expand_indirect_vtbls_init (binfo, true_exp, decl_ptr)
     tree binfo;
     tree true_exp, decl_ptr;
{
  tree type = BINFO_TYPE (binfo);

  /* This function executes during the finish_function() segment,
     AFTER the auto variables and temporary stack space has been marked
     unused...If space is needed for the virtual function tables,
     some of them might fit within what the compiler now thinks
     are available stack slots... These values are actually initialized at
     the beginnning of the function, so when the automatics use their space,
     they will overwrite the values that are placed here. Marking all
     temporary space as unavailable prevents this from happening. */

  mark_all_temps_used();

  if (TYPE_USES_VIRTUAL_BASECLASSES (type))
    {
      rtx fixup_insns = NULL_RTX;
      tree vbases = CLASSTYPE_VBASECLASSES (type);
      struct vbase_info vi;
      vi.decl_ptr = (true_exp ? build_unary_op (ADDR_EXPR, true_exp, 0) 
		     : decl_ptr);
      vi.vbase_types = vbases;

      dfs_walk (binfo, dfs_find_vbases, unmarked_new_vtablep, &vi);

      /* Initialized with vtables of type TYPE.  */
      for (; vbases; vbases = TREE_CHAIN (vbases))
	{
	  tree addr;

	  addr = convert_pointer_to_vbase (TREE_TYPE (vbases), vi.decl_ptr);

	  /* Do all vtables from this virtual base.  */
	  /* This assumes that virtual bases can never serve as parent
	     binfos.  (in the CLASSTYPE_VFIELD_PARENT sense)  */
	  expand_direct_vtbls_init (vbases, TYPE_BINFO (BINFO_TYPE (vbases)),
				    1, 0, addr);

	  /* Now we adjust the offsets for virtual functions that
	     cross virtual boundaries on an implicit upcast on vf call
	     so that the layout of the most complete type is used,
	     instead of assuming the layout of the virtual bases from
	     our current type.  */

	  if (flag_vtable_thunks)
	    {
	      /* We don't have dynamic thunks yet!
		 So for now, just fail silently.  */
	    }
	  else
	    {
	      tree vbase_offsets = NULL_TREE;
	      push_to_sequence (fixup_insns);
	      fixup_virtual_upcast_offsets (vbases,
					    TYPE_BINFO (BINFO_TYPE (vbases)),
					    1, 0, addr, vi.decl_ptr,
					    type, vbases, &vbase_offsets);
	      fixup_insns = get_insns ();
	      end_sequence ();
	    }
	}

      if (fixup_insns)
	{
	  extern tree in_charge_identifier;
	  tree in_charge_node = lookup_name (in_charge_identifier, 0);
	  if (! in_charge_node)
	    {
	      warning ("recoverable internal compiler error, nobody's in charge!");
	      in_charge_node = integer_zero_node;
	    }
	  in_charge_node = build_binary_op (EQ_EXPR, in_charge_node, integer_zero_node);
	  expand_start_cond (in_charge_node, 0);
	  emit_insns (fixup_insns);
	  expand_end_cond ();
	}

      dfs_walk (binfo, dfs_clear_vbase_slots, marked_new_vtablep, 0);
    }
}

/* get virtual base class types.
   This adds type to the vbase_types list in reverse dfs order.
   Ordering is very important, so don't change it.  */

static tree
dfs_get_vbase_types (binfo, data)
     tree binfo;
     void *data;
{
  tree *vbase_types = (tree *) data;

  if (TREE_VIA_VIRTUAL (binfo) && ! BINFO_VBASE_MARKED (binfo))
    {
      tree new_vbase = make_binfo (integer_zero_node, binfo,
				   BINFO_VTABLE (binfo),
				   BINFO_VIRTUALS (binfo));
      TREE_CHAIN (new_vbase) = *vbase_types;
      TREE_VIA_VIRTUAL (new_vbase) = 1;
      *vbase_types = new_vbase;
      SET_BINFO_VBASE_MARKED (binfo);
    }
  SET_BINFO_MARKED (binfo);
  return NULL_TREE;
}

/* Return a list of binfos for the virtual base classes for TYPE, in
   depth-first search order.  The list is freshly allocated, so
   no modification is made to  the current binfo hierarchy.  */

tree
get_vbase_types (type)
     tree type;
{
  tree vbase_types;
  tree vbases;
  tree binfo;

  binfo = TYPE_BINFO (type);
  vbase_types = NULL_TREE;
  dfs_walk (binfo, dfs_get_vbase_types, unmarkedp, &vbase_types);
  dfs_walk (binfo, dfs_unmark, markedp, 0);
  /* Rely upon the reverse dfs ordering from dfs_get_vbase_types, and now
     reverse it so that we get normal dfs ordering.  */
  vbase_types = nreverse (vbase_types);

  /* unmark marked vbases */
  for (vbases = vbase_types; vbases; vbases = TREE_CHAIN (vbases))
    CLEAR_BINFO_VBASE_MARKED (vbases);

  return vbase_types;
}

/* If we want debug info for a type TYPE, make sure all its base types
   are also marked as being potentially interesting.  This avoids
   the problem of not writing any debug info for intermediate basetypes
   that have abstract virtual functions.  Also mark member types.  */

void
note_debug_info_needed (type)
     tree type;
{
  tree field;

  if (current_template_parms)
    return;
    
  if (TYPE_BEING_DEFINED (type))
    /* We can't go looking for the base types and fields just yet.  */
    return;

  /* We can't do the TYPE_DECL_SUPPRESS_DEBUG thing with DWARF, which
     does not support name references between translation units.  Well, we
     could, but that would mean putting global labels in the debug output
     before each exported type and each of its functions and static data
     members.  */
  if (write_symbols == DWARF_DEBUG || write_symbols == DWARF2_DEBUG)
    return;

  dfs_walk (TYPE_BINFO (type), dfs_debug_mark, dfs_debug_unmarkedp, 0);
  for (field = TYPE_FIELDS (type); field; field = TREE_CHAIN (field))
    {
      tree ttype;
      if (TREE_CODE (field) == FIELD_DECL
	  && IS_AGGR_TYPE (ttype = target_type (TREE_TYPE (field)))
	  && dfs_debug_unmarkedp (TYPE_BINFO (ttype), 0))
	note_debug_info_needed (ttype);
    }
}

/* Subroutines of push_class_decls ().  */

/* Returns 1 iff BINFO is a base we shouldn't really be able to see into,
   because it (or one of the intermediate bases) depends on template parms.  */

static int
dependent_base_p (binfo)
     tree binfo;
{
  for (; binfo; binfo = BINFO_INHERITANCE_CHAIN (binfo))
    {
      if (currently_open_class (TREE_TYPE (binfo)))
	break;
      if (uses_template_parms (TREE_TYPE (binfo)))
	return 1;
    }
  return 0;
}

static void
setup_class_bindings (name, type_binding_p)
     tree name;
     int type_binding_p;
{
  tree type_binding = NULL_TREE;
  tree value_binding;

  /* If we've already done the lookup for this declaration, we're
     done.  */
  if (IDENTIFIER_CLASS_VALUE (name))
    return;

  /* First, deal with the type binding.  */
  if (type_binding_p)
    {
      type_binding = lookup_member (current_class_type, name,
				    /*protect=*/2,
				    /*want_type=*/1);
      if (TREE_CODE (type_binding) == TREE_LIST 
	  && TREE_TYPE (type_binding) == error_mark_node)
	/* NAME is ambiguous.  */
	push_class_level_binding (name, type_binding);
      else
	pushdecl_class_level (type_binding);
    }

  /* Now, do the value binding.  */
  value_binding = lookup_member (current_class_type, name,
				 /*protect=*/2,
				 /*want_type=*/0);

  if (type_binding_p
      && (TREE_CODE (value_binding) == TYPE_DECL
	  || (TREE_CODE (value_binding) == TREE_LIST
	      && TREE_TYPE (value_binding) == error_mark_node
	      && (TREE_CODE (TREE_VALUE (value_binding))
		  == TYPE_DECL))))
    /* We found a type-binding, even when looking for a non-type
       binding.  This means that we already processed this binding
       above.  */
    my_friendly_assert (type_binding_p, 19990401);
  else
    {
      if (TREE_CODE (value_binding) == TREE_LIST 
	  && TREE_TYPE (value_binding) == error_mark_node)
	/* NAME is ambiguous.  */
	push_class_level_binding (name, value_binding);
      else
	{
	  if (BASELINK_P (value_binding))
	    /* NAME is some overloaded functions.  */
	    value_binding = TREE_VALUE (value_binding);
	  pushdecl_class_level (value_binding);
	}
    }
}

/* Push class-level declarations for any names appearing in BINFO that
   are TYPE_DECLS.  */

static tree
dfs_push_type_decls (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  tree type;
  tree fields;

  type = BINFO_TYPE (binfo);
  for (fields = TYPE_FIELDS (type); fields; fields = TREE_CHAIN (fields))
    if (DECL_NAME (fields) && TREE_CODE (fields) == TYPE_DECL
	&& !(!same_type_p (type, current_class_type)
	     && template_self_reference_p (type, fields)))
      setup_class_bindings (DECL_NAME (fields), /*type_binding_p=*/1);

  /* We can't just use BINFO_MARKED because envelope_add_decl uses
     DERIVED_FROM_P, which calls get_base_distance.  */
  SET_BINFO_PUSHDECLS_MARKED (binfo);

  return NULL_TREE;
}

/* Push class-level declarations for any names appearing in BINFO that
   are not TYPE_DECLS.  */

static tree
dfs_push_decls (binfo, data)
     tree binfo;
     void *data;
{
  tree type;
  tree method_vec;
  int dep_base_p;

  type = BINFO_TYPE (binfo);
  dep_base_p = (processing_template_decl && type != current_class_type
		&& dependent_base_p (binfo));
  if (!dep_base_p)
    {
      tree fields;
      for (fields = TYPE_FIELDS (type); fields; fields = TREE_CHAIN (fields))
	if (DECL_NAME (fields) 
	    && TREE_CODE (fields) != TYPE_DECL
	    && TREE_CODE (fields) != USING_DECL)
	  setup_class_bindings (DECL_NAME (fields), /*type_binding_p=*/0);
	else if (TREE_CODE (fields) == FIELD_DECL
		 && ANON_UNION_TYPE_P (TREE_TYPE (fields)))
	  dfs_push_decls (TYPE_BINFO (TREE_TYPE (fields)), data);
	  
      method_vec = (CLASS_TYPE_P (type) 
		    ? CLASSTYPE_METHOD_VEC (type) : NULL_TREE);
      if (method_vec)
	{
	  tree *methods;
	  tree *end;

	  /* Farm out constructors and destructors.  */
	  end = TREE_VEC_END (method_vec);

	  for (methods = &TREE_VEC_ELT (method_vec, 2);
	       *methods && methods != end;
	       methods++)
	    setup_class_bindings (DECL_NAME (OVL_CURRENT (*methods)), 
				  /*type_binding_p=*/0);
	}
    }

  CLEAR_BINFO_PUSHDECLS_MARKED (binfo);

  return NULL_TREE;
}

/* When entering the scope of a class, we cache all of the
   fields that that class provides within its inheritance
   lattice.  Where ambiguities result, we mark them
   with `error_mark_node' so that if they are encountered
   without explicit qualification, we can emit an error
   message.  */

void
push_class_decls (type)
     tree type;
{
  struct obstack *ambient_obstack = current_obstack;
  search_stack = push_search_level (search_stack, &search_obstack);

  /* Build up all the relevant bindings and such on the cache
     obstack.  That way no memory is wasted when we throw away the
     cache later.  */
  push_cache_obstack ();

  /* Enter type declarations and mark.  */
  dfs_walk (TYPE_BINFO (type), dfs_push_type_decls, unmarked_pushdecls_p, 0);

  /* Enter non-type declarations and unmark.  */
  dfs_walk (TYPE_BINFO (type), dfs_push_decls, marked_pushdecls_p, 0);

  /* Undo the call to push_cache_obstack above.  */
  pop_obstacks ();

  current_obstack = ambient_obstack;
}

/* Here's a subroutine we need because C lacks lambdas.  */

static tree
dfs_unuse_fields (binfo, data)
     tree binfo;
     void *data ATTRIBUTE_UNUSED;
{
  tree type = TREE_TYPE (binfo);
  tree fields;

  for (fields = TYPE_FIELDS (type); fields; fields = TREE_CHAIN (fields))
    {
      if (TREE_CODE (fields) != FIELD_DECL)
	continue;

      TREE_USED (fields) = 0;
      if (DECL_NAME (fields) == NULL_TREE
	  && TREE_CODE (TREE_TYPE (fields)) == UNION_TYPE)
	unuse_fields (TREE_TYPE (fields));
    }

  return NULL_TREE;
}

void
unuse_fields (type)
     tree type;
{
  dfs_walk (TYPE_BINFO (type), dfs_unuse_fields, unmarkedp, 0);
}

void
pop_class_decls ()
{
  /* We haven't pushed a search level when dealing with cached classes,
     so we'd better not try to pop it.  */
  if (search_stack)
    search_stack = pop_search_level (search_stack);
}

void
print_search_statistics ()
{
#ifdef GATHER_STATISTICS
  fprintf (stderr, "%d fields searched in %d[%d] calls to lookup_field[_1]\n",
	   n_fields_searched, n_calls_lookup_field, n_calls_lookup_field_1);
  fprintf (stderr, "%d fnfields searched in %d calls to lookup_fnfields\n",
	   n_outer_fields_searched, n_calls_lookup_fnfields);
  fprintf (stderr, "%d calls to get_base_type\n", n_calls_get_base_type);
#else /* GATHER_STATISTICS */
  fprintf (stderr, "no search statistics\n");
#endif /* GATHER_STATISTICS */
}

void
init_search_processing ()
{
  gcc_obstack_init (&search_obstack);
  _vptr_name = get_identifier ("_vptr");
}

void
reinit_search_statistics ()
{
#ifdef GATHER_STATISTICS
  n_fields_searched = 0;
  n_calls_lookup_field = 0, n_calls_lookup_field_1 = 0;
  n_calls_lookup_fnfields = 0, n_calls_lookup_fnfields_1 = 0;
  n_calls_get_base_type = 0;
  n_outer_fields_searched = 0;
  n_contexts_saved = 0;
#endif /* GATHER_STATISTICS */
}

#define scratch_tree_cons expr_tree_cons

static tree
add_conversions (binfo, data)
     tree binfo;
     void *data;
{
  int i;
  tree method_vec = CLASSTYPE_METHOD_VEC (BINFO_TYPE (binfo));
  tree *conversions = (tree *) data;

  for (i = 2; i < TREE_VEC_LENGTH (method_vec); ++i)
    {
      tree tmp = TREE_VEC_ELT (method_vec, i);
      tree name;

      if (!tmp || ! DECL_CONV_FN_P (OVL_CURRENT (tmp)))
	break;

      name = DECL_NAME (OVL_CURRENT (tmp));

      /* Make sure we don't already have this conversion.  */
      if (! IDENTIFIER_MARKED (name))
	{
	  *conversions = scratch_tree_cons (binfo, tmp, *conversions);
	  IDENTIFIER_MARKED (name) = 1;
	}
    }
  return NULL_TREE;
}

tree
lookup_conversions (type)
     tree type;
{
  tree t;
  tree conversions = NULL_TREE;

  if (TYPE_SIZE (type))
    bfs_walk (TYPE_BINFO (type), add_conversions, 0, &conversions);

  for (t = conversions; t; t = TREE_CHAIN (t))
    IDENTIFIER_MARKED (DECL_NAME (OVL_CURRENT (TREE_VALUE (t)))) = 0;

  return conversions;
}

struct overlap_info 
{
  tree compare_type;
  int found_overlap;
};

/* Check whether the empty class indicated by EMPTY_BINFO is also present
   at offset 0 in COMPARE_TYPE, and set found_overlap if so.  */

static tree
dfs_check_overlap (empty_binfo, data)
     tree empty_binfo;
     void *data;
{
  struct overlap_info *oi = (struct overlap_info *) data;
  tree binfo;
  for (binfo = TYPE_BINFO (oi->compare_type); 
       ; 
       binfo = BINFO_BASETYPE (binfo, 0))
    {
      if (BINFO_TYPE (binfo) == BINFO_TYPE (empty_binfo))
	{
	  oi->found_overlap = 1;
	  break;
	}
      else if (BINFO_BASETYPES (binfo) == NULL_TREE)
	break;
    }

  return NULL_TREE;
}

/* Trivial function to stop base traversal when we find something.  */

static tree
dfs_no_overlap_yet (binfo, data)
     tree binfo;
     void *data;
{
  struct overlap_info *oi = (struct overlap_info *) data;
  return !oi->found_overlap ? binfo : NULL_TREE;
}

/* Returns nonzero if EMPTY_TYPE or any of its bases can also be found at
   offset 0 in NEXT_TYPE.  Used in laying out empty base class subobjects.  */

int
types_overlap_p (empty_type, next_type)
     tree empty_type, next_type;
{
  struct overlap_info oi;

  if (! IS_AGGR_TYPE (next_type))
    return 0;
  oi.compare_type = next_type;
  oi.found_overlap = 0;
  dfs_walk (TYPE_BINFO (empty_type), dfs_check_overlap,
	    dfs_no_overlap_yet, &oi);
  return oi.found_overlap;
}

struct bfv_info {
  tree vbases;
  tree var;
};

static tree
dfs_bfv_queue_p (binfo, data)
     tree binfo;
     void *data;
{
  struct bfv_info *bfvi = (struct bfv_info *) data;

  /* Use the real virtual base class objects, not the placeholders in
     the usual hierarchy.  */
  if (TREE_VIA_VIRTUAL (binfo))
    return binfo_member (BINFO_TYPE (binfo), bfvi->vbases);
  
  return binfo;
}

/* Passed to dfs_walk_real by binfo_for_vtable; determine if bvtable
   comes from BINFO.  */

static tree
dfs_bfv_helper (binfo, data)
     tree binfo;
     void *data;
{
  struct bfv_info *bfvi = (struct bfv_info *) data;

  if (BINFO_VTABLE (binfo) == bfvi->var)
    return binfo;
  return NULL_TREE;
}

/* Given a vtable VAR, determine which binfo it comes from.  */

tree
binfo_for_vtable (var)
     tree var;
{
  tree type;
  struct bfv_info bfvi;

  type = DECL_CONTEXT (var);
  bfvi.vbases = CLASSTYPE_VBASECLASSES (type);
  bfvi.var = var;
  return dfs_walk_real (TYPE_BINFO (type),
			0, dfs_bfv_helper, dfs_bfv_queue_p, &bfvi);
}
