#ifndef __NGX_FUNC_H__
#define __NGX_FUNC_H__

// 函数声明-------------------------------------------

// 字符串相关函数
void Rtrim(char *string);
void Ltrim(char *string);

// 设置可执行程序标题相关函数
void ngx_init_setproctitle();
void ngx_setproctitle(const char *title);

#endif