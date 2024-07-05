gcc barbershop.c ../zemaphore/zemaphore.c -o barbershop -lpthread
# gcc -o master-worker master-worker.c -lpthread
./barbershop $1 $2 