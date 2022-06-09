// Parto de drv3 para realizar las modificaciones a rea y write para ver qué hace
// estoy yendo en la dirección de clipboard.buffer_entrada ..!!

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/delay.h>

#define mem_size 128

static dev_t first; 		// Global variable for the first device number
static struct cdev c_dev; 	// Global variable for the character device structure
static struct class *cl; 	// Global variable for the device class

char *buffer_entrada;
char *buffer_salida;
int contador1;
int contador2;

static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "SdeC_drv4: open()\n");
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "SdeC_drv4: close()\n");
    return 0;
}

// ssize_t resulta ser una palabra con signo.
// Por lo tanto, puede ocurrir que devuelva un número negativo. Esto sería un error. 
// Pero un valor de retorno no negativo tiene un significado adicional. 
// Para my_read sería el número de bytes leídos

// Cuando hago un $ cat /dev/SdeC_drv3, se convoca a my_read.!!
// my_read lee "len" bytes, los guarda en "buf" y devuelve la cantidad leida, que puede
// ser menor, pero nunca mayor que len.

// En SdeC_drv3, devuelve cero. Dado que es un archivo, esto significa no hay mas datos ó EOF.
// Lo que tendría que ocurrir es que el device escriba sobre buf para que el usuario pueda 
// obtener una lectura no nula.

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "SdeC_drv4: read()\n");

        switch (buffer_entrada[0])
        {
            case 'a': /* Sensar Humedad */
		        contador1++;
	 	        if(contador1 == 30)
                    contador1 = 0;
                             
                memset(buffer_salida, NULL, mem_size);
                sprintf(buffer_salida, "%d", contador1);
                pr_info("Valor del contador1: %s \n", buffer_salida);

                if(copy_to_user(buf, buffer_salida, mem_size) != 0){

                    printk(KERN_INFO "Valor del contador1: %s", buffer_salida);
                    return -EFAULT;
                }

                printk(KERN_INFO "leyendo contador1 \n");
                return 0; 

                        
            case 'b': /* Sensar Temperatura */
                
                sprintf(buffer_salida, "%d", contador2);
                if(copy_to_user(buf, buffer_salida, len) != 0){
                    printk(KERN_INFO "Error al devolver contador2\n");
                    return -EFAULT;
                }

                printk(KERN_INFO "leyendo contador2 \n");
                return 0;

            default:
                if (*off == 0)
                {
                    if ( copy_to_user(buf, "5", 1) != 0)
                        return -EFAULT;
                    else
                    {
                        (*off)++;
                        return 1;
                    }
                }
                else
                    return 0;
        }
}
// my_write escribe "len" bytes en "buf" y devuelve la cantidad escrita, que debe ser igual "len".
// Cuando hago un $ echo "bla bla bla..." > /dev/SdeC_drv3, se convoca a my_write.!!

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "SdeC_drv4: write()\n");    

    memset(buffer_entrada, '\0', mem_size);

    if ( copy_from_user(buffer_entrada, buf , len) != 0 )    
        return -EFAULT;
    else{
        if(buffer_entrada[0] == 'a'){
            contador1++;
        }
        else if(buffer_entrada[0] == 'b'){
            contador2+=2;
        }       
        return len;
    }
            
}

static struct file_operations pugs_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

static int __init drv4_init(void) /* Constructor */
{
    int ret;
    contador1 = 0;
    contador2 = 0;
    struct device *dev_ret;

//alocamos memoria para el buffer de entrada
    if ((buffer_entrada = kmalloc(mem_size, GFP_KERNEL)) == 0)
    {
        pr_info(KERN_INFO "Error al alocar memoria para buffer_entrada\n");
        return -1;
    }
   
    if ((buffer_salida = kmalloc(mem_size, GFP_KERNEL)) == 0)
    {
        pr_info(KERN_INFO "Error al alocar memoria para buffer_salida\n");
        return -1;
    }
 
    printk(KERN_INFO "SdeC_drv4: Registrado exitosamente..!!\n");

    if ((ret = alloc_chrdev_region(&first, 0, 1, "SdeC_drv4")) < 0)
    {
        return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, "SdeC_drive")))
    {
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "SdeC_drv4")))
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&c_dev, &pugs_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }
    return 0;
}

static void __exit drv4_exit(void) /* Destructor */
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "SdeC_drv4: dice Adios mundo cruel..!!\n");
}

module_init(drv4_init);
module_exit(drv4_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cátedra Sistemas de Computación");
MODULE_DESCRIPTION("Nuestro cuarto driver de SdeC");
