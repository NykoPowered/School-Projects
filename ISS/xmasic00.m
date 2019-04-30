%1
[y,Fs]=audioread('xmasic00.wav');
pocet=length(y);
cas=pocet/Fs;

%2
figure(2)
s = y(8:16:end);
for i = 1 : length(s)
    if s(i) > 0
        s(i) = 1;
    else
        s(i) = 0;
    end
end
xlabel('t[ms]');
ylabel('dlzka');
hold on
plot(y(1:Fs/80));
stem((1:10)*20,s(1:10));
hold off

%3
figure(3);
B = [0.0192 -0.0185 -0.0185 0.0192];
A = [1.000 -2.8870 2.7997 -0.9113];
zplane(B,A);

%4
figure(4)
freq=(0:pocet-1)/pocet*Fs/2;
H=freqz(B,A,pocet);
plot(freq,abs(H));
xlabel('f[Hz]');
ylabel('H[dB]');

%5
figure(5)
set(gca,'XTick',[0 20 40 60 80 100 120 140 160 180 200]);
set(gca,'XTickLabel',[0 0.002 0.004 0.006 0.008 0.01 0.012 0.014 0.016 0.018 0.02]);
xlabel('t[ms]');
ylabel('dlzka');
ssn=filter(B,A,y);
plot(ssn(1:Fs/80));

%6
figure(6)
xshifted=circshift(ssn,-16);
plot(xshifted(1:Fs/80));
s2 = xshifted(8:16:end);
for i = 1 : length(s2)
    if s2(i) > 0
        s2(i) = 1;
    else
        s2(i) = 0;
    end
end    
hold on
plot(xshifted(1:Fs/80));
stem((1:10)*20,s2(1:10));

%7
counter = 0;
for i = 1:length(s)-1
    if s(i)~= s2(i)
        counter = counter+1
    end
end
fprintf("Chýb je: %d a chybovost je %f",counter,100*counter/length(s));

%8
figure(8)
hold on
fouriertrans=fft(y);
v =(0:pocet-1)*Fs/pocet;
m = abs(fouriertrans);
plot(v(1:pocet/2),m(1:pocet/2));
fouriertrans2=fft(ssn);
v2 =(0:pocet-1)*Fs/pocet;
m2 = abs(fouriertrans2);
plot(v2(1:pocet/2),m2(1:pocet/2));
hold off
xlabel('f [Hz]');

%9
figure(9)
hustota=hist(y,50);
plot(hustota/pocet);
disp(sum(hustota/pocet));
set(gca,'XTick',[0 5 10 15 20 25 30 35 40 45 50])
set(gca,'XTickLabel',[-1 -0.8 -0.6 -0.4 -0.2 0 0.2 0.4 0.6 0.8 1])
xlabel('s[n]')
ylabel('hustota')

%10
[Rv,lag]=xcorr(y,'biased');
plot(lag,Rv);
xlim([-50 50]);
%11
Rv0=Rv(0+pocet);
Rv1=Rv(1+pocet);
Rv16=Rv(16+pocet);

%12
x = linspace(min(y),max(y),100);
[h,p,r]=hist2opt(y(1:pocet-1),y(2:pocet),x);
imagesc(-x,x,p);
colorbar;

%13

%14
disp(r);

function [h,p,r] = hist2(y1,y2,x); 
L = length(x); 
N = length(y1);  
h = zeros(L,L); out = 0;
xcol = x(:); bigx = repmat(xcol,1,N); 
yr = y1(:)'; bigy = repmat(yr,L,1);
[dummy,ind1] = min(abs(bigy - bigx)); 
yr = y2(:)'; bigy = repmat(yr,L,1);
[dummy,ind2] = min(abs(bigy - bigx)); 
for ii=1:N,
  d1 = ind1(ii);   d2 = ind2(ii); 
  h(d1,d2) = h(d1,d2) + 1; 
end
surf = (x(2) - x(1))^2;
p = h / N / surf;  
x = x(:); X1 = repmat(x,1,L);
x = x'; X2 = repmat(x,L,1); 
r = sum(sum (X1 .* X2 .* p)) * surf;
check = sum(sum (p)) * surf;
disp(['hist2: check -- 2d integral should be 1 and is ' num2str(check)]);
end

