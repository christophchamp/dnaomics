/*
 * File name: test_nm_cmd.c
 * SOURCE: http://en.wikipedia.org/wiki/Nm_%28Unix%29
 * For C code compile with: 
 * gcc -c test_nm_cmd.c
 * nm test_nm_cmd.o
 *
 * For C++ code compile with:
 * g++ -c test_nm_cmd.c
 */
int global_var;
int global_var_init = 26;
 
static int static_var;
static int static_var_init = 25;
 
static int static_function()
{
        return 0;
}
 
int global_function(int p)
{
        static int local_static_var;
        static int local_static_var_init=5;
 
        local_static_var = p;
 
        return local_static_var_init + local_static_var;
}
 
int global_function2()
{
        int x;
        int y;
        return x+y;
}
 
#ifdef __cplusplus
extern "C"
#endif
void non_mangled_function()
{
        // I do nothing
}
 
int main(void)
{
        global_var = 1;
        static_var = 2;
 
        return 0;
}

