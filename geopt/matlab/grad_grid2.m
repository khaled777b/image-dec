function D = grad_grid2( width, height )

[ X Y ] = meshgrid( 1:width, 1:height );

dXdX(:,:,1) = X(:,1:width-1);
dXdX(:,:,2) = X(:,2:width);
dYdX = Y(:,1:width-1);

dYdY(:,:,1) = Y(1:height-1,:);
dYdY(:,:,2) = Y(2:height,:);
dXdY = X(1:height-1,:);

dX(:,:,1) = sub2ind([height width], dYdX, dXdX(:,:,1));
dX(:,:,2) = sub2ind([height width], dYdX, dXdX(:,:,2));

dY(:,:,1) = sub2ind([height width], dYdY(:,:,1), dXdY);
dY(:,:,2) = sub2ind([height width], dYdY(:,:,2), dXdY);

nve = (height-1)*width;
nhe = (width-1)*height;

vi = [ 1:nve; 1:nve ];
hi = [ 1:nhe; 1:nhe ] + nve;

dX = reshape( dX, nhe, 2 )';
dY = reshape( dY, nve, 2 )';

vc = [ -1*ones(1,nve); ones(1,nve) ];
hc = [ -1*ones(1,nhe); ones(1,nhe) ];

is = [ vi(:); hi(:) ];
js = [ dY(:); dX(:) ];
vs = [ vc(:); hc(:) ];

D = sparse( is, js, vs, nve+nhe, width*height );
