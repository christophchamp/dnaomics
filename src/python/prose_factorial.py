# Calculating the factorial of a number 
# is a good way to practice recursion. Below 
# are examples of both recursive and iterative 
# approaches to calculating a factorial. Of course 
# python has its own builtin math module that 
# can also take care of factorials. 
 
import math
 
# A recursive function to calculate the 
# factorial of N 
def recursiveFactorial(n):
    if n == 1:
        return n
    return n * recursiveFactorial(n - 1)
 
# Iterative calculation of the factorial 
# of N 
def iterativeFactorial(n):
    result = 1
    while n > 1:
        result *= n
        n = n-1
    return result
 
# Builtin python math.factorial of N 
def pythonLib(n):
    return math.factorial(n)
 
print("Iterative factorial of 5: ", iterativeFactorial(5))
print("Recursive factorial of 5: ", recursiveFactorial(5))
print("math.factorial of 5: ", pythonLib(5))
 
print("Iterative factorial of 10: ", iterativeFactorial(10))
print("Recursive factorial of 10: ", recursiveFactorial(10))
print("math.factorial of 10: ", pythonLib(10))
 
print("Iterative factorial of 15: ", iterativeFactorial(15))
print("Recursive factorial of 15: ", recursiveFactorial(15))
print("math.factorial of 15: ", pythonLib(15))
 
 
# my results: 
# Iterative factorial of 5:  120 
# Recursive factorial of 5:  120 
# math.factorial of 5:  120 
# Iterative factorial of 10:  3628800 
# Recursive factorial of 10:  3628800 
# math.factorial of 10:  3628800 
# Iterative factorial of 15:  1307674368000 
# Recursive factorial of 15:  1307674368000 
# math.factorial of 15:  1307674368000
