/******************************************************************************
* Copyright (C) 2017 by Ben Heberlein
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. This file
* was created for the University of Colorado Boulder course Advanced Practical
* Embedded Software Development. Ben Heberlein and the University of Colorado 
* are not liable for any misuse of this material.
*
*******************************************************************************/
/**
 * @file memsort.c
 * @brief Memory sorter system call implementation
 * 
 * This file provides the system call implementation for a memory sorter. The
 * function takes in a pointer to a buffer, a size, and a pointer to a sorted 
 * buffer as parameters. More details are given in the function definition.
 *
 * @author Ben Heberlein
 * @date September 16 2017
 * @version 1.0
 *
 */

#include "linux/types.h"
#include "linux/printk.h"
#include "linux/stddef.h"
#include "linux/errno.h"
#include "linux/uaccess.h"
#include "linux/slab.h"

/**
 * @brief Memory sorter system call
 * 
 * This function sorts the input buffer from largest to smallest, and stores
 * the result in the output buffer. The function will validate all input
 * parameters, and print information to the kernel buffer showing success
 * or failure.
 *
 * @param buf A pointer to a int32_t buffer of input data
 * @param size The size of the buffer as a uin16_t
 * @param sbuf A pointer to a int32_t buffer of sorted data
 *
 * @return A kernel error code from errno.h or errno-base.h
 */
SYSCALL_DEFINE3(memsort, int32_t __user *, buf, int32_t, size, int32_t __user *, sbuf) {
    printk(KERN_INFO, "memsort system call starting.\n");

    long ret = 0;

    /* Validate arguments */
    if (buf == NULL) {
        printk(KERN_WARNING, "memsort failed with a null pointer argument.\n");
        return -EFAULT;
    }

    if (sbuf == NULL) {
        printk(KERN_WARNING, "memsort failed with a null pointer argument.\n");
        return -EFAULT;
    }

    if (size <= 0) {
        printk(KERN_WARNING, "memsort failed with a negative size argument.\n");
        return -EINVAL;
    }

    if (access_ok(VERIFY_WRITE, buf, size)) {
        printk(KERN_WARNING, "memsort buffer is not in userspace.\n");
        return -EFAULT;
    }

    if (access_ok(VERIFY_WRITE, sbuf, size)) {
        printk(KERN_WARNING, "memsort buffer is not in userspace.\n");
        return - EFAULT;
    }

    /* Print size of buffer */
    printk(KERN_INFO, "memsort size of buffer is %d bytes.\n", size*sizeof(int32_t));

    /* Allocate kernel memory for sorting */
    int32_t *sort = kmalloc(size, GFP_KERNEL);
    if (sort == NULL) {
        printk(KERN_WARNING, "memsort failed to allocate memory.\n");
    }    


    /* Copy source buffer */
    if (copy_from_user(buf, sort, size*sizeof(int32_t))) {
        printk(KERN_WARNING, "memsort failed to copy from user space.\n");
        kfree(sort);
        return -EFAULT;
    }

    /* Sort memory */
    int32_t temp = 0;
    for (int i = 0; i < size; i++) {
        for (int j = i+1; j < size; j++) {
            if (sort[i] < sort[j]) {
                temp = sort[i];
                sort[i] = sort[j];
                sort[j] = temp;
            }
        }
    }

    /* Display result */
    printk(KERN_INFO, "memsort successfully sorted numbers: ");
    for (int i = 0; i < size; i++) {
        printk(KERN_CONT, "%d ", sort[i]);
    }
    printk(KERN_CONT, ".\n");

    /* Copy back to user */
    if (copy_to_user(sbuf, sort, size*sizeof(int32_t))) {
        printk(KERN_WARNING, "memsort failed to copy to user space.\n");
        kfree(sort);
        return -EFAULT;
    } 

    /* Free memory */
    kfree(sort);

    /* Goodbye */
    printk(KERN_INFO, "memsort successfully completed.\n");

    return 0;
}































