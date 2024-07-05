echo "Running test-toggle.c"
g++ test-toggle.c zemaphore.c -o test-toggle -lpthread
./test-toggle
