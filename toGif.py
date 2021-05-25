import os, imageio

images = []

for num in range(0, 40):
	file_path = "Output\\" + str(num) + ".bmp"
	
	images.append(imageio.imread(file_path))
	
	print(num)
	
imageio.mimsave("out.gif", images, duration = 0.15)