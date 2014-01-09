#!/usr/bin/env ruby

require 'fileutils'
include FileUtils
$prefix = "$HOME/lib/libtiff"

def main
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
	if File.exist?("tiff-4.0.3.tar.gz") == true
		print blue('=>')
		print " Libtiff already exists in this folder. Redownload? (Y/n): "
		dL = gets
		dL = dL.chomp
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
		system "curl --progress-bar -o tiff-4.0.3.tar.gz http://download.osgeo.org/libtiff/tiff-4.0.3.tar.gz"
	rescue 
		print red('=>')
		print " Error Downloading libTIFF\n"
		exit
	end
end

def unTar
	print blue('=>')
	print " Untaring...\n"
	begin
		system "tar -xf tiff-4.0.3.tar.gz"
	rescue
		print red('=>')
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
		mkdir("./libtiff")
		cd("./tiff-4.0.3")
	rescue
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
	print blue("=>")
	print " Cleaning up...\n"
	cd("..")
	system "rm -rf libtiff tiff-4.0.3*" 
end

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
