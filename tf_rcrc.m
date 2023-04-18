clc
clear all

R1 = 2e3;
C1 = 1e-6;
R2 = 1e3;
C2 = 1e-6;
%% 
% Opcion 1: Realizar el desarrollo matematico de la ecuación diferencial 
% del circuito y obtener la función transferencia en el dominio de Laplace
% Hs = 1 / ((R1*C1*R2*C2)*s^2 + (R1*C1+R1*C2+R2*C2)*s + 1)
s = tf('s');
Hs_1 = 1 / ((R1*C1*R2*C2)*s^2 + (R1*C1+R1*C2+R2*C2)*s + 1)
[num_1 den_1] = tfdata(Hs_1, 'v');


% Hs_1 =
%  
%              1
%   -----------------------
%   2e-06 s^2 + 0.005 s + 1
%  
% Continuous-time transfer function.

%%
[A, B, C, D] = tf2ss(num_1, den_1);
[num_3, den_3] = ss2tf(A, B, C, D);
Hs_3 = C*(s*eye(length(A))-A)^(-1)*B + D;

%%
h = 0.1;
Hz_1 = c2d(Hs_1, h, 'zoh');

[numz_1, denz_1] = tfdata(Hz_1, 'v');

[Phi_1, Rho_1, Cz_1, Dz_1] = tf2ss(numz_1, denz_1)

%%
bode(Hs_1);

%%
GP = pidtune(Hs_1,'PID')
%GP =
%  
%              1          
%   Kp + Ki * --- + Kd * s
%              s          
% 
%   with Kp = 1.64, Ki = 561, Kd = 0.000672
%  
% Continuous-time PID controller in parallel form.
%%
figure;
step(Hs_1, Hz_1)

