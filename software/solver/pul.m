
t0 = 1;
tp = 2.16e-3;
B0 = 40e-6;
B1 = 1e-3;
M0 = [0; 0; B0];
M0 /= sqrt(dot(M0, M0));

opts = odeset( ...
  'RelTol', 1e-4, ...
  'AbsTol', 1e-4, ...
  'InitialStep', 1e-6, ...
  'MaxStep', 1e-1);

function dMdt = bloch (t, M, t0, tf, B0, B1)
  g = 42.57748e6;
  tau = 4;
  B = field(t, t0, tf, B0, B1);
  dMdt = g .* cross(M, B) - (1 / tau) .* (M - [0; 0; 1]);
end

function B = field (t, t0, tp, B0, B1)
  if (isscalar(t))
    vB0 = [0; 0; B0];
    vB1 = [B1; 0; 0];
    if t < t0
      B = vB0;
    elseif t < t0 + tp
      f = B0 * 42.57748e6;
      B = vB0 + vB1 * sin(2 * pi * f * (t - t0));
    else
      B = vB0;
    end
  elseif (isvector(t))
    B = zeros(length(t), 3);
    for i = 1 : length(t)
      B(i,:) = field(t(i), t0, tp, B0, B1)';
    end
  else
    error('invalid time data type');
  end
end

[t, M] = ode23(@bloch, [1, 2], M0, opts, t0, tp, B0, B1);
data = [t, M, field(t, t0, tp, B0, B1)];

dv = fix(clock());
dstr = sprintf('pul-%04d%02d%02d.txt', dv(1), dv(2), dv(3));
save('-ascii', dstr, 'data');

