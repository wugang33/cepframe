//#define _GNU_SOURCE
#include <memory.h>
#include "sigsegv.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <ucontext.h>
#include <dlfcn.h>
#include <execinfo.h>
#ifndef NO_CPP_DEMANGLE
#include <cxxabi.h>
#endif
#if defined(REG_RIP)
//#define SIGSEGV_STACK_IA64
#define REGFORMAT "%016lx"
#elif defined(REG_EIP)
#define SIGSEGV_STACK_X86
#define REGFORMAT "%08x"
#else
#define SIGSEGV_STACK_GENERIC
#define REGFORMAT "%x"
#endif

void get_addr(char * addr_) {
    char *p_addr, *p_addr2;
    char addr[512];
    char addr_real[24];
    char cd_line[1024];
    memset(cd_line, '\0', sizeof (cd_line));
    memset(addr, '\0', sizeof (addr));
    memset(addr_real, '\0', sizeof (addr_real));
    p_addr = strrchr(addr_, '(');
    strncpy(addr, addr_, (p_addr - addr_));
    p_addr = strrchr(addr_, '[');
    p_addr2 = strrchr(addr_, ']');
    strncpy(addr_real, p_addr + 1, (p_addr2 - p_addr) - 1);
    sprintf(cd_line, "addr2line -e %s %s", addr, addr_real);
    system(cd_line);
}

static void signal_segv(int signum, siginfo_t* info, void*ptr) {
    static const char *si_codes[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};
    size_t i;
    ucontext_t *ucontext = (ucontext_t*) ptr;
#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)  
    int f = 0;
    Dl_info dlinfo;
    void **bp = 0;
    void *ip = 0;
#else  
    void *bt[20];
    char **strings;
    size_t sz;
#endif  
    printf("\n ### signal=[%d] ### \n", signum);

    if (signum == SIGPIPE) {
        printf("\n >>> Ingore: Broken pipe (POSIX) <<< \n");
    } else if (signum == SIGSEGV) {
        printf("\n >>> SIGSEGV 11 C 无效的内存引用 系统正在退出！ <<< \n");
    } else if (signum == SIGILL) {
        printf("\n >>> SIGILL 4 C 非法指令 系统正在退出！ <<< \n");
    } else if (signum == SIGABRT) {
        printf("\n >>> SIGABRT 6 C 由abort(3)发出的退出指令 系统正在退出！ <<< \n");
    } else if (signum == SIGHUP) {
        printf("\n >>> Ingore: Hangup (POSIX) SIGHUP 1 A 终端挂起或者控制进程终止！ <<< \n");
    } else if (signum == SIGINT) {
    } else if (signum == SIGUSR1) {
        printf("\n ### OK, system will shutdown by your command! ### \n");
    }
    fprintf(stderr, "Segmentation Fault!\n");
    fprintf(stderr, "info.si_signo = %d\n", signum);
    fprintf(stderr, "info.si_errno = %d\n", info->si_errno);
    fprintf(stderr, "info.si_code  = %d (%s)\n", info->si_code, si_codes[info->si_code]);
    fprintf(stderr, "info.si_addr  = %p\n", info->si_addr);
    for (i = 0; i < NGREG; i++)
        fprintf(stderr, "reg[%02d]= 0x" REGFORMAT "\n", i, ucontext->uc_mcontext.gregs[i]);
#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)
#if defined(SIGSEGV_STACK_IA64)  
    ip = (void*) ucontext->uc_mcontext.gregs[REG_RIP];
    bp = (void**) ucontext->uc_mcontext.gregs[REG_RBP];
#elif defined(SIGSEGV_STACK_X86)
    ip = (void*) ucontext->uc_mcontext.gregs[REG_EIP];
    bp = (void**) ucontext->uc_mcontext.gregs[REG_EBP];
#endif
    fprintf(stderr, "Stack trace:\n");
    while (bp && ip) {
        if (!dladdr(ip, &dlinfo))
            break;
        const char *symname = dlinfo.dli_sname;
#ifndef NO_CPP_DEMANGLE    
        int status;
        char *tmp = __cxxabiv1::__cxa_demangle(symname, NULL, 0, &status);
        if (status == 0 && tmp) symname = tmp;
#endif    
        fprintf(stderr, "%2d: %p <%s> (%s)\n", ++f, ip, symname, dlinfo.dli_fname);
#ifndef NO_CPP_DEMANGLE    
        if (tmp)
            free(tmp);
#endif    
        if (dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
            break;
        ip = bp[1];
        bp = (void**) bp[0];
    }
#else  
    fprintf(stderr, "Stack trace (non-dedicated):\n");
    sz = backtrace(bt, 20);

    strings = backtrace_symbols(bt, sz);
    for (i = 0; i < sz; ++i) {

        fprintf(stderr, "%s\n", strings[i]);
        get_addr(strings[i]);
    }
#endif  
    fprintf(stderr, "End of stack trace\n");
    exit(-1);
}

int setup_sigsegv() {
    struct sigaction action;
    memset(&action, 0, sizeof (action));
    action.sa_sigaction = signal_segv;
    action.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &action, NULL) < 0) {
        perror("sigaction");
        return 0;
    }
    return 1;
}

// #ifndef SIGSEGV_NO_AUTO_INIT
#ifdef __VER4SIGSEGV_PRINT_BACKTRACE__

static void __attribute((constructor))

init(void) {
    setup_sigsegv();
}
#endif
