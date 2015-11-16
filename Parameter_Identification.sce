loadmatfile('-ascii','C:\Users\user\Desktop\Newtest.csv')
initms=Newtest(1,1)
initcount=Newtest(1,2)
L = max(size(Newtest))
t=0:01:100
omega(j)=initcount
j = 1 
for i = 1:L
    if Newtest(i,2) > initcount then
         omega(j)=((Newtest(i,2)-initcount)/(Newtest(i,1)-initms)*1000)*(%pi/180)
  //       omega(j,1)= t +1;
         initcount=Newtest(i,2)
         initms=Newtest(i,1)
         j = j+1
    //else omega= initcount*(%pi/180)
    //    omega(j,1)= t +1;
 end,
end
Fs=1000
plot(omega)
