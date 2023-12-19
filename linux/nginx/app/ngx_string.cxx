#include <stdio.h>
#include <string.h>

// 一些和字符串处理相关的函数，准备放这里

// 截取字符串尾部空格
void Rtrim(char *string) {
  size_t len = 0;
  if (string == NULL) return;

  len = strlen(string);
  while (len > 0 && string[len - 1] == ' ')  // 位置换一下
    string[--len] = 0;
  return;
}

// 截取字符串首部空格
void Ltrim(char *string) {
  size_t len = 0;
  len = strlen(string);
  char *p_tmp = string;
  if ((*p_tmp) != ' ')  // 不是以空格开头
    return;
  // 找第一个不为空格的
  while ((*p_tmp) != '\0') {
    if ((*p_tmp) == ' ')
      p_tmp++;
    else
      break;
  }
  // 全是空格
  if ((*p_tmp) == '\0') {
    *string = '\0';
    return;
  }
  char *p_tmp2 = string;
  while ((*p_tmp) != '\0') {
    (*p_tmp2) = (*p_tmp);
    p_tmp++;
    p_tmp2++;
  }
  (*p_tmp2) = '\0';
  return;
}