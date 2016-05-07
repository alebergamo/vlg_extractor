function M = load_bit_vector(fileName)
% function that loads a BIN descriptor into Matlab

fid = fopen(fileName);
if fid < 0
    error(['Error during opening the file ' fileName]);
end

data = fread(fid, inf, '*uchar');
b1 = bitget(data, 1)';
b2 = bitget(data, 2)';
b3 = bitget(data, 3)';
b4 = bitget(data, 4)';
b5 = bitget(data, 5)';
b6 = bitget(data, 6)';
b7 = bitget(data, 7)';
b8 = bitget(data, 8)';

M = zeros(8*length(data), 1);
M(1:8:end) = b8;
M(2:8:end) = b7;
M(3:8:end) = b6;
M(4:8:end) = b5;
M(5:8:end) = b4;
M(6:8:end) = b3;
M(7:8:end) = b2;
M(8:8:end) = b1;


fclose(fid);

end
