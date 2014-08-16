
t0 = 1;
tf = 400e-6;
B0 = 40e-6;
B1 = 1e-3;
M0 = [B1; 0; B0];
M0 /= sqrt(dot(M0, M0));

opts = odeset( ...
  'RelTol', 1e-4, ...
  'AbsTol', 1e-4, ...
  'InitialStep', 1e-6, ...
  'MaxStep', 1e-1);

data = [];
vtf = linspace(0, 100e-3, 51)';

function dMdt = bloch (t, M, t0, tf, B0, B1)
  g = 42.57748e6;
  tau = 4;
  B = field(t, t0, tf, B0, B1);
  dMdt = g .* cross(M, B) - (1 / tau) .* (M - [0; 0; 1]);
end

function B = field (t, t0, tf, B0, B1)
  if (isscalar(t))
    vB0 = [0; 0; B0];
    vB1 = [B1; 0; 0];
    if t < t0
      B = vB0 + vB1;
    elseif t < t0 + tf
      B = vB0 + vB1 * (1 - (t - t0) / tf);
    else
      B = vB0;
    end
  elseif (isvector(t))
    B = zeros(length(t), 3);
    for i = 1 : length(t)
      B(i,:) = field(t(i), t0, tf, B0, B1)';
    end
  else
    error('invalid time data type');
  end
end

function y = decay (x, p)
  y = p(1) .* exp(p(2) .* x) + p(3);
end

for i = 1 : length(vtf)
  tf = vtf(i);

  fprintf(stdout, 'calculating for fall time = %.3e\n', tf);
  fflush(stdout);

  [t, M] = ode23(@bloch, [1, 2], M0, opts, t0, tf, B0, B1);

  Mx = M(:,1);
  My = M(:,2);
  Mz = M(:,3);

  tidx = find(abs(t - t0 - tf) == min(abs(t - t0 - tf)));
  data = [data; [tf, Mx(tidx)]];
end

X = data(:,1);
Y = data(:,2);
pin = [1; 1; 0];
stol = 1e-6;
niter = 100;
wt = ones(size(Y));
dp = 1e-3 * ones(size(pin));

[Mhat, p] = leasqr(X, Y, pin, @decay, stol, niter, wt, dp, 'dfdp', {});

data = [data, Mhat];

dv = fix(clock());
dstr = sprintf('multisol-%04d%02d%02d.txt', dv(1), dv(2), dv(3));
save('-ascii', dstr, 'data');

pstr = sprintf('multisol-parms-%04d%02d%02d.txt', dv(1), dv(2), dv(3));
save('-ascii', pstr, 'p');

