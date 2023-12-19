#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ngx_c_conf.h"  //和配置文件处理相关的类,名字带c_表示和类有关
#include "ngx_func.h"  //各种函数声明
#include "ngx_signal.h"

// 和设置标题有关的全局量
char **g_os_argv;        // 原始命令行参数数组,在main中会被赋值
char *gp_envmem = NULL;  // 指向自己分配的env环境变量的内存
int g_environlen = 0;    // 环境变量所占内存大小

int main(int argc, char *const *argv) {
  g_os_argv = (char **)argv;

  /*
      for (int i = 0; environ[i]; i++)
      {
          printf("environ[%d]地址=%x    " ,i,(unsigned int)((unsigned
     long)environ[i])); printf("environ[%d]内容=%s\n" ,i,environ[i]);
      }
      printf("--------------------------------------------------------");
  */
  ngx_init_setproctitle();  // 把环境变量搬家

  /*
      for (int i = 0; environ[i]; i++)
      {
          printf("environ[%d]地址=%x    " ,i,(unsigned int)((unsigned
     long)environ[i])); printf("environ[%d]内容=%s\n" ,i,environ[i]);
      }
  */
  // 我们在main中，先把配置读出来，供后续使用
  CConfig *p_config = CConfig::GetInstance();  // 单例类
  if (p_config->Load("nginx.conf") ==
      false)  // 把配置文件内容载入到内存，配置文件与可执行程序在同一个目录，所以可以直接载入配置文件
  {
    printf("配置文件载入失败，退出!\n");
    exit(1);
  }

  //---------------------------------------------------------
  // 获取配置文件信息的用法
  // int port = p_config->GetIntDefault("ListenPort",0); //0是缺省值
  // printf("port=%d\n",port);
  // const char *pDBInfo = p_config->GetString("DBInfo");
  // if(pDBInfo != NULL)
  //{
  //   printf("DBInfo=%s\n",pDBInfo);
  //}

  //---------------------------------------------------------
  // 测试内存泄漏可以写如下代码，让程序顺利退出
  //   if (true) {
  //     printf("为了检测程序内存泄漏而进行的程序退出，再见!\n");
  //     return 0;
  //   }

  //---------------------------------------------------------
  // printf("argc=%d,argv[0]=%s\n",argc,argv[0]);
  // strcpy(argv[0],"ce");
  // strcpy(argv[0],"c2212212121322324323e"); //肯定会覆盖掉相当一部分内存
  // printf("environ[0]=%s\n" , environ[0]);
  // printf("environ[1]=%s\n" , environ[1]);
  // printf("environ[2]=%s\n" , environ[2]);
  // printf("environ[3]=%s\n" , environ[3]);
  // printf("environ[4]=%s\n" , environ[4]);

  // 验证argv指向的内存和environ指向的内存紧挨着
  // for(int i = 0; i < argc; ++i)
  //{
  //     printf("argv[%d]地址=%x    " ,i,(unsigned int)((unsigned
  //     long)argv[i])); printf("argv[%d]内容=%s\n",i,argv[i]);
  // }
  // 下面环境变量随便输出2个
  // for(int i = 0; i < 2; ++i)
  //{
  //     printf("evriron[%d]地址=%x    " ,i,(unsigned int)((unsigned
  //     long)environ[i])); printf("evriron[%d]内容=%s\n" ,i,environ[i]);
  // }

  // 要保证所有命令行参数从下面这行代码开始都不再使用，才能调用ngx_setproctitle函数，因为调用后，命令行参数的内容可能会被覆盖掉
  ngx_setproctitle("nginx: master process");

  // myconf();
  // mysignal();

  for (;;) {
    sleep(1);  // 休息1秒
    printf("休息1秒\n");
  }

  // 对于因为设置可执行程序标题导致的环境变量分配的内存，我们应该释放
  if (gp_envmem) {
    delete[] gp_envmem;
    gp_envmem = NULL;
  }
  printf("程序退出，再见!\n");
  return 0;
}
