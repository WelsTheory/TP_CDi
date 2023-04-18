function [Theta P] = identificacionRLS(n, u, y, lastP, lastTheta)
  Phi = [];
  for i=n:-1:1
    Phi = [Phi y(i)];
  end
  for j=(n+1):-1:1
    Phi = [Phi u(j)];
  end
  P = lastP - lastP*Phi'*Phi*lastP/(1+Phi*lastP*Phi');
  K = lastP*Phi'/(1+Phi*lastP*Phi');
  Theta = lastTheta + K*(y(n+1)-Phi*lastTheta);
end