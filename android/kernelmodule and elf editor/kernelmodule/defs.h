#define TO_STRING2(x) #x
#define TO_STRING(x) TO_STRING2(x)

#define F_SHOW(x) static int my_proc_show##x(struct seq_file *m,void *v){ \
	unsigned int numberr = 0; \
	dbg_read(numberr,DBG_##x); \
	seq_printf(m,TO_STRING(DBG_##x)" %X\n",numberr); \
	return 0; \
}

#define F_WRITE(x) static ssize_t my_proc_write##x(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos){ \
	unsigned int *tmp = kzalloc((count+1),GFP_KERNEL); \
	if(!tmp)return -ENOMEM; \
	if(copy_from_user(tmp,buffer,count)){ \
		kzfree(tmp); \
		return EFAULT; \
	} \
	dbg_write(*tmp, DBG_##x); \
	kzfree(tmp); \
	return count; \
}

#define F_FUNC(x) static int my_proc_open##x(struct inode *inode,struct file *file){ \
	return single_open(file,my_proc_show##x,NULL); \
}

#define F_OPS(x) static struct file_operations my_fops##x = { \
	.owner = THIS_MODULE, \
	.open = my_proc_open##x, \
	.release = single_release, \
	.read = seq_read, \
	.llseek = seq_lseek, \
	.write = my_proc_write##x \
};

#define CREATE_ENRTY(x) struct proc_dir_entry *entry##x;

#define SET_ENTRY(x) entry##x = proc_create("erasto_"#x,0444,NULL,&my_fops##x); \
	if(!entry##x){ \
		return -1; \
	}else{ \
		printk(KERN_INFO "create erasto_"#x" file successfully\n"); \
}

#define REMOVE_ENTRY(x) remove_proc_entry("erasto_"#x,NULL);