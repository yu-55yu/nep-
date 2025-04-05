sum=0
n=1
m=1
while 1:
    if abs (m)<1e-6:
        break
    else:
        sum+=m
        n+=2
        m=(-(-1)**((n+1)/2))/n
        
print("pi=", sum*4)