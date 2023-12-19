#ifndef __NGX_GBLDEF_H__
#define __NGX_GBLDEF_H__

// 一些比较通用的定义放在这里

// 结构定义
typedef struct {
  char ItemName[50];
  char ItemContent[500];
} CConfItem, *LPCConfItem;

// 外部全局量声明
extern char **g_os_argv;
extern char *gp_envmem;
extern int g_environlen;

#endif
