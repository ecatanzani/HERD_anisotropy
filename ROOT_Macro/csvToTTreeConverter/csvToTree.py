#!/usr/bin/python3
import os
from sys import exit
import csv
import numpy as np
from datetime import datetime
from argparse import ArgumentParser
from ROOT import TFile, TTree, TMath

start = datetime.now()

words_in_line = 54
glat_word_counter = 23
thetas_word_counter = 19

def main(args=None):
	parser = ArgumentParser(usage="Usage: %(prog)s [options]", description="CSV to TTree converter")
	parser.add_argument("-l", "--list", type=str,dest='list', help='Input CSV DATA list')
	parser.add_argument("-o", "--output", type=str,dest='output', help='output TFile')
	parser.add_argument("-v", "--verbose", dest='verbose', default=False,action='store_true', help='run in high verbosity mode')
	opts = parser.parse_args(args)
	
	# Create glon e glat variables
	glat = np.empty((1), dtype="float")
	glon = np.empty((1), dtype="float")
	thetas = np.empty((1), dtype="float")
	phi = np.empty((1), dtype="float")
	events = np.empty((1), dtype="int")
	geo_lat = np.empty((1), dtype="float")
	geo_lon = np.empty((1), dtype="float")

	# Create new TFile
	RTI_output_file = TFile(opts.output, "RECREATE")
	if RTI_output_file.IsZombie():
		print('Error writing output TFile [{}]'.format(opts.output))
		exit()

	# Create final Tree
	RTI_tree = TTree("RTI_tree", "AMS RTI TTree")

	# Branch the TTree
	RTI_tree.Branch("glat", glat, "glat/D")
	RTI_tree.Branch("glon", glon, "glon/D")
	RTI_tree.Branch("thetas", thetas, "thetas/D")
	RTI_tree.Branch("phi", phi, "phi/D")
	RTI_tree.Branch("events", events, "events/I")
	RTI_tree.Branch("geo_lat", geo_lat, "geo_lat/D")
	RTI_tree.Branch("geo_lon", geo_lon, "geo_lon/D")
	
	# Parse input list
	with open(opts.list, 'r') as input_list:
		csv_files = input_list.readlines()
		for file in csv_files:
			with open(file.rstrip("\n"), 'r') as tmp_file:
				csv_reader = csv.reader(tmp_file, delimiter=' ', quoting=csv.QUOTE_NONE)
				line_count = 0
				for row in csv_reader:
					if line_count != 0:
						if int(row[1]):
							thetas[0] = float(row[18])
							phi[0] = float(row[19])
							glat[0] = float(row[22])
							glon[0] = float(row[23])
							events[0] = int(row[24])
							geo_lat[0] = thetas*TMath.RadToDeg()
							geo_lon[0] = phi*TMath.RadToDeg()
							RTI_tree.Fill()
					line_count += 1
				if opts.verbose:
					print('Has been read {} lines [{}]'.format(line_count, file.rstrip("\n")))
	RTI_tree.Write()
	RTI_output_file.Close()

if __name__ == "__main__":
	main()
	print(datetime.now()-start)