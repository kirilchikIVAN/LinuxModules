#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "my_device"
#define BUF_SIZE 1024
#define FIELD_SIZE 30
#define PB_SIZE 100

static int major_number;
static char buffer[BUF_SIZE];
static int buffer_len = 0;
static int user_is_read = 0;

struct user_data {
    char name[FIELD_SIZE];
    char surname[FIELD_SIZE];
    char age[FIELD_SIZE];
    char phone[FIELD_SIZE];
    char email[FIELD_SIZE];
};

static struct user_data phonebook[PB_SIZE];
static int phonebook_size = 0;
static int out_user = -1;

static void get_user_info(char* name, char* surname, char* age, char* phone, char* email) {
    int name_start = -1;
    int surname_start = -1;
    int age_start = -1;
    int phone_start = -1;
    int email_start = -1;

    int name_len = -1;
    int surname_len = -1;
    int age_len = -1;
    int phone_len = -1;
    int email_len = -1;

    int start_idx = 0;
    int len;

    for (int i = 0; i < buffer_len; ++i) {
        if (buffer[i] == ' ') {
            len = i - start_idx;
            if (name_start == -1) {
                name_start = start_idx;
                name_len = len;
            } else if (surname_start == -1) {
                surname_start = start_idx;
                surname_len = len;
            } else if (age_start == -1) {
                age_start = start_idx;
                age_len = len;
            } else if (phone_start == -1) {
                phone_start = start_idx;
                phone_len = len;
            }
            start_idx = i + 1;
        }
    }
    email_start = start_idx;
    email_len = buffer_len - start_idx;

    for (int i = name_start; i < name_start + name_len; ++i) {
        name[i - name_start] = buffer[i];
    }
    for (int i = surname_start; i < surname_start + surname_len; ++i) {
        surname[i - surname_start] = buffer[i];
    }
    for (int i = age_start; i < age_start + age_len; ++i) {
        age[i - age_start] = buffer[i];
    }
    for (int i = phone_start; i < phone_start + phone_len; ++i) {
        phone[i - phone_start] = buffer[i];
    }
    for (int i = email_start; i < email_start + email_len; ++i) {
        email[i - email_start] = buffer[i];
    }

    name[name_len] = '\0';
    surname[surname_len] = '\0';
    age[age_len] = '\0';
    phone[phone_len] = '\0';
    email[email_len] = '\0';
}

static int findd_user(char* str) {
    int eq;
    int surname_len;
    int user_idx = -1;
    int str_len = strlen(str);

    printk(KERN_INFO "Find start\n");
    if (phonebook_size == 0) {
        printk(KERN_INFO "Find err: no users\n");
        return -1;
    }

    for (int i = 0; i < phonebook_size; ++i) {
        surname_len = strlen(phonebook[i].surname);
        if (surname_len != str_len) {
            continue;
        }

        eq = 1;
        for (int j = 0; j < surname_len; ++j) {
            if (str[j] != phonebook[i].surname[j]) {
                eq = 0;
                break;
            }
        }
        if (eq == 1) {
            user_idx = i;
            break;
        }
    }

    printk(KERN_INFO "Find end\n");
    return user_idx;
}

static void out_user_to_buffer(void) {
    int name_len = strlen(phonebook[out_user].name);
    int surname_len = strlen(phonebook[out_user].surname);
    int age_len = strlen(phonebook[out_user].age);
    int phone_len = strlen(phonebook[out_user].phone);
    int email_len = strlen(phonebook[out_user].email);
    int bi = 0;

    for (int i = 0; i < name_len; ++i) {
        buffer[bi++] = phonebook[out_user].name[i];
    }
    buffer[bi++] = ' ';
    for (int i = 0; i < surname_len; ++i) {
        buffer[bi++] = phonebook[out_user].surname[i];
    }
    buffer[bi++] = ' ';
    for (int i = 0; i < age_len; ++i) {
        buffer[bi++] = phonebook[out_user].age[i];
    }
    buffer[bi++] = ' ';
    for (int i = 0; i < phone_len; ++i) {
        buffer[bi++] = phonebook[out_user].phone[i];
    }
    buffer[bi++] = ' ';
    for (int i = 0; i < email_len; ++i) {
        buffer[bi++] = phonebook[out_user].email[i];
    }
    buffer[bi++] = '\n';
    buffer[bi] = '\0';
    buffer_len = bi;
}

static long add_user(void) {
    printk(KERN_INFO "Add start\n");
    if (phonebook_size >= PB_SIZE) {
        printk(KERN_INFO "Add err: too many users\n");
        return -1;
    }
    get_user_info(
            phonebook[phonebook_size].name,
            phonebook[phonebook_size].surname,
            phonebook[phonebook_size].age,
            phonebook[phonebook_size].phone,
            phonebook[phonebook_size].email
    );
    if (findd_user(phonebook[phonebook_size].surname) != -1) {
        printk(KERN_INFO "Add err: user already exists\n");
        return -1;
    }
    ++phonebook_size;
    out_user = -1;

    printk(KERN_INFO "Add end\n");
    return 0;
}

static long gett_user(void) {
    printk(KERN_INFO "Get start\n");
    if (phonebook_size == 0) {
        printk(KERN_INFO "Get err: no users\n");
        return -1;
    }

    out_user = findd_user(buffer);

    if (out_user == -1) {
        printk(KERN_INFO "Get err: no such user\n");
        return -1;
    }

    printk(KERN_INFO "Get end\n");
    return 0;
}

static long del_user(void) {
    int del_user = 0;
    printk(KERN_INFO "Del start\n");
    if (phonebook_size == 0) {
        printk(KERN_INFO "Del err: no users\n");
        return -1;
    }

    del_user = findd_user(buffer);
    if (del_user == -1) {
        printk(KERN_INFO "Del err: no such user\n");
        return -1;
    }

    for (int i = del_user; i < phonebook_size - 1; ++i) {
        phonebook[i] = phonebook[i + 1];
    }

    --phonebook_size;
    out_user = -1;

    printk(KERN_INFO "Del end\n");
    return 0;
}

static int device_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode* inode, struct file* file) {
    printk(KERN_INFO "Device released\n");
    return 0;
}

static ssize_t device_read(struct file* file, char* user_buffer, size_t length, loff_t* offset) {
    ssize_t bytes_to_copy;

    if (user_is_read == 1) {
        return 0;
    }

    if (out_user == -1) {
        printk(KERN_INFO "Out user isn't set\n");
    }

    out_user_to_buffer();

    bytes_to_copy = buffer_len < length ? buffer_len : length;
    if (copy_to_user(user_buffer, buffer, bytes_to_copy) != 0)
        return -EFAULT;

    user_is_read = 1;

    return bytes_to_copy;
}

static ssize_t device_write(struct file* file, const char* user_buffer, size_t length, loff_t* offset) {
    ssize_t bytes_to_copy = length < BUF_SIZE ? length : BUF_SIZE;
    int space_cnt = 0;
    int last_space = -1;
    if (copy_from_user(buffer, user_buffer, bytes_to_copy) != 0)
        return -EFAULT;

    buffer_len = bytes_to_copy - 1;
    buffer[buffer_len] = '\0';


    for (int i = 0; i < buffer_len; ++i) {
        if (buffer[i] == ' ') {
            ++space_cnt;
            last_space = i;
        }
    }

    if (space_cnt == 0) {
        user_is_read = 0;
        gett_user();
    } else if (space_cnt == 1) {
        if ((buffer[0] == 'd' && buffer[1] == 'e' && buffer[2] == 'l')) {
            ++last_space;
            for (int i = 0; last_space + i < buffer_len; ++i) {
                buffer[i] = buffer[last_space + i];
            }
            buffer_len = buffer_len - last_space;
            buffer[buffer_len] = '\0';
            del_user();
        } else {
            printk(KERN_ALERT "No such option\n");
        }
    } else if (space_cnt == 4) {
        add_user();
    } else {
        printk(KERN_ALERT "No such option\n");
    }

    return bytes_to_copy;
}

static struct file_operations fops = {
        .open = device_open,
        .release = device_release,
        .read = device_read,
        .write = device_write,
};

static int __init my_module_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }

    printk(KERN_INFO "Successfully registered device with major number %d\n", major_number);

    return 0;
}

static void __exit my_module_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivanchik Kirill");
MODULE_DESCRIPTION("Phonebook kernel module");
