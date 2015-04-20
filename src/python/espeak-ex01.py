import subprocess

def execute_unix(inputcommand):
   p = subprocess.Popen(inputcommand, stdout=subprocess.PIPE, shell=True)
   (output, err) = p.communicate()
   return output

a = "Some amazing words of wisdom."

# write out to wav file 
b = 'espeak -w temp.wav "%s" 2>>/dev/null' % a  

# speak aloud
c = 'espeak -ven+f3 -k5 -s150 --punct="<characters>" "%s" 2>>/dev/null' % a #speak aloud

execute_unix(b) 
execute_unix(c) 
