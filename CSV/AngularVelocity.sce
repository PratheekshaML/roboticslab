loadmatfile('-ascii','C:\Users\user\Desktop\Newtest.csv')
initms=Newtest(1,1)
initcount=Newtest(1,2)
L = max(size(Newtest))
j=1 
u = 1
A = 1
omega(j)=initcount
Fs = 1000;
t = 0:1/Fs:22;
f = linspace(0,Fs,length(t)); // Create frequency vectors
n = length(t);
for i = 1:L
    if Newtest(i,2) > initcount then
         omega(j)=((Newtest(i,2)-initcount)/(Newtest(i,1)-initms)*1000)*(%pi/180)
         initcount=Newtest(i,2)
         initms=Newtest(i,1)
         u = omega(j)
         j = j+1
         else
         omega(j)= u
         j=j+1
 end,
end
//plot(omega')
X = fft(omega')./(length(omega')/2); // Creating frequency response of the signal

hz = iir(5,'lp','butt',0.3/Fs,[])

[hzm,fr]=frmag(hz,1200);

fr2 = fr.*Fs;

Omega = flts(omega',hz);

Y = fft(Omega)./(length(omega')/2);

//plot(Omega)

q_est = Omega($);
k_est = q_est/A;
y_0 = Omega(1);
y_inf = q_est;
y_max = max(Omega);
Overshoot = abs(y_max - y_inf)/abs(y_0 - y_inf);
xi_est = sqrt(log(Overshoot)^2/(%pi^2 + log(Overshoot)^2));

// Natural frequency
alpha = 5;
Ts = -1;
Omega_at_Ts = -1;
for i=size(Omega,2):-1:1
  if abs(q_est - Omega(i)) < alpha/100*q_est
    Ts = t(i);
    Omega_at_Ts = Omega(i);
  else
    break;
  end
end
if Ts == -1
  disp('No settling time available');
end
N_bar = 1/sqrt(1 - xi_est^2);
o_n_est = (log(alpha/100) - log(N_bar))/(-xi_est*Ts);

// Natural frequency from output period
[OmegaFirstMax, FirstMax] = max(Omega);
[OmegaFirstMin, FirstMin] = min(Omega(FirstMax:$));
FirstMin = FirstMin + FirstMax - 1;
[OmegaSecondMax, SecondMax] = max(Omega(FirstMin:$));
SecondMax = SecondMax + FirstMin - 1;
T = t(SecondMax) - t(FirstMax);
o_n_est2 = 2*%pi/(T*sqrt(1 - xi_est^2));


// Simulation
s = poly(0, 's');
G_est = k_est/(s^2/o_n_est^2 + 2*xi_est/o_n_est*s + 1);
G_est = syslin('c', G_est);
Omega_est = csim(ones(1,size(t,2))*A, t, G_est);

G_est2 = k_est/(s^2/o_n_est2^2 + 2*xi_est/o_n_est2*s + 1);
G_est2 = syslin('c', G_est2);
Omega_est2 = csim(ones(1,size(t,2))*A, t, G_est2);

scf(1);
clf;
plot(Omega);
plot(q_est*(100 - alpha)/100, 'r--');
plot(q_est*(100 + alpha)/100, 'r--');
plot([Ts, Ts], [0, Omega_at_Ts], 'k');
plot(Omega_est, 'g');
plot(Omega_est2, 'm');
plot([t(FirstMax), t(FirstMin), t(SecondMax)], [Omega(FirstMax), Omega(FirstMin), Omega(SecondMax)], 'bo');
