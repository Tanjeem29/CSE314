gcc barbershop_lock.c ../zemaphore/zemaphore.c -o barbershop_lock -lpthread
# gcc -o master-worker master-worker.c -lpthread
./barbershop_lock $1 $2 