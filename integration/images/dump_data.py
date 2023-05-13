# Yong Da Li
# Saturday, May 14, 2022
# dumping .mat file to text files


from matplotlib import pyplot as plt
import scipy.io			# to load matlab *.mat files
import numpy as np

# keep the python file in the same folder as all the data
in_file = "Indian_pines.mat"
out_file = "data_full.txt"

# load data as dict
matlab_data = scipy.io.loadmat(in_file)
data = matlab_data['indian_pines']

# slicing 3rd axis shows a x-y top-down view of the landscape
# data[:,:,0] is pixelated, slicing into a higher index looks better
# plt.figure(1)
# plt.title("slicing 3rd index")
# plt.imshow(data[:,:,10])	
# plt.show()

# write header 
[xlen, ylen, zlen] = data.shape
f = open(out_file, "w+")
f.write(f"{zlen}, {xlen}, {ylen},\n")		# write z-dimension first

# slice z = get frames, write each frame, go to next frame
for z in range (0, len(data[:,:,0])):
	# get frame
	frame = data[:,:,z]

	# write each row in frame
	for row in frame:
		print_line = ", ".join(str(elem) for elem in row)
		f.write(print_line) 	# expects string format
		f.write(",") 			# write end of line comma
		f.write("\n")

f.close()
