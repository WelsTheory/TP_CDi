function [Theta] = identificacionLS(n, u, y)
  Y = y(n+1:length(y));
  Phi = [];
  for i=n:-1:1
    Phi = [Phi y(i:(length(y)+i-n-1))];
  end
  for j=(n+1):-1:1
    Phi = [Phi u(j:(length(y)+j-n-1))];
  end
  Theta = (Phi'*Phi)^(-1)*Phi'*Y;
end