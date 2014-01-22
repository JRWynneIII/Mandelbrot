#!/usr/bin/env ruby

#Include the module that will allow manipulation of files/directories
require 'fileutils'
include FileUtils
#set the prefix global variable to where the library will be installed. change this to change where you want 
#to extract and compile libTIFF
$prefix = "$HOME/lib/libtiff"

def main
	#Get command line arguments. TO CHANGE the prefix variable via commandline, use 
	# ./buildTiff.rb --prefix /path/to/directory
	opt1 = ARGV[0]
	arg1 = ARGV[1]

	if opt1 == "--prefix"
		$prefix = arg1
	else
		if opt1 != nil 
			print blue('=>')
			puts " Invalid Argument. Exiting"
			exit
		end
	end

	#Check to see if the tarball is already downloaded. If you have no internet connection, just drop the tarball (downloaded
	# from another computer) in the same directory as this file and select 'n' when it asks if you'd like to redownload it
	if File.exist?("tiff-4.0.3.tar.gz") == true
		print blue('=>')
		print " Libtiff already exists in this folder. Redownload? (Y/n): "
		dL = gets
		dL = dL.chomp
		#Check to see if user wants to redownload it. If so untar it, make the required directories
		#compile it then clean up unneeded files and directories. If user doesn't want to redownload it
		#then skip the download file stage
		if dL == 'y' || dL == 'Y'
			downloadFile()
			unTar()
			makeDirs()
			install()
			clean()
		else
			unTar()
			makeDirs()
			install()
			clean()
		end
	else
		downloadFile()
		unTar()
		makeDirs()
		install()
		clean()
	end
end

def downloadFile
	print blue('=>')
	print " Downloding libTIFF\n"
	begin
		#Download the tarball from the libtiff website with curl and just display a progress to the user
		system "curl --progress-bar -o tiff-4.0.3.tar.gz http://download.osgeo.org/libtiff/tiff-4.0.3.tar.gz"
	rescue 
		#Oops something went wrong. You might want to check to see if curl exists on the system and that 
		#you have good internet connetion. Also make sure you have permission to write here!
		print red('=>')
		print " Error Downloading libTIFF\n"
		exit
	end
end

def unTar
	#Untar the file!!
	print blue('=>')
	print " Untaring...\n"
	begin
		system "tar -xf tiff-4.0.3.tar.gz"
	rescue
		print red('=>')
		#check to see if the tiff directory exists. I.e. its already been extracted. This might happen if the build process was
		#Killed half way through. Just in case this is the problem, it removes it and re downloads it and then untars it
		if File.exist?("tiff-4.0.3") == true
			system "rm -r tiff-4.0.3*"
			downloadFile()
		else
			print " Error untarring the file!"
		end
	end
end

def makeDirs
	begin
		#make the directories needed for the library. Then change into the tiff direcotry so that its ready for compiling
		mkdir("./libtiff")
		cd("./tiff-4.0.3")
	rescue
		#this will check if the directory exists or if there is an issue. if so it removes everything and re tries it
		if File.exist?("./libtiff") == true
			system "rm -r libtiff"
			makeDirs()
		else
			print "Error making directories! (Do you have permissions here?)"
			exit
		end
	end
end

def install
	#runs through the standard configure, make, make install process and hides the output as well as passes the right flags to configure
	#so that it will work on Titan and Rhea
	print blue("=>")
	print " Configuring libTiff\n"
	system "./configure --prefix=#{$prefix} --disable-jpeg --disable-zlib > ~/lib/configlog.txt"
	print blue("=>")
	print " Running make\n"
	system "make &> ~/lib/makeLog.txt"
	print blue("=>")
	print " Running make install\n"
	system "make install &> ~/lib/installLog.txt"
	print green("=> Success!\n")
end

def clean
	#remove all unnessicary files and directories
	print blue("=>")
	print " Cleaning up...\n"
	cd("..")
	system "rm -rf libtiff tiff-4.0.3*" 
end

#These functions are only used to change color of text for visual effects. Disregard and don't change anything below this comment
def colorize(text, color_code)
  "#{color_code}#{text}\e[0m"
end

def red(text)
	colorize(text, "\e[31m")
end

def green(text)
	colorize(text, "\e[32m")
end

def blue(text)
	colorize(text, "\e[34m")
end

if __FILE__ == $PROGRAM_NAME
	main()
end
