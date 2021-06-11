#include <mpi.h>
#include <iostream>
#include <malloc.h>
#include <math.h>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;


bool Sinir_Durum_Kontrol_Fonksiyonu(int satir, int sutun, int i, int j) {
	if ((i == 0) || (j == 0) || (i == (satir - 1)) || (j == (sutun - 1))) {
		return false;
	}
	return true;
}


int main(int argc, char *argv[]) {
	
	double baslangic = 0.0, bitis = 0.0;

	int size, 
		counter,
		myRank, 
		satir, 
		sutun, 
		elemanSayisi,
		islenecekElemanSayisi=0,
		diziIndisi,
		alimBoyutu,
		yeniPixelDegeri,
		filtreMatrisi[3][3] = { {-2,-1,0},{-1,1,1},{0,1,2} };

	int kuzey,
		kuzeyBati,
		kuzeyDogu,
		merkez,
		bati,
		dogu,
		guney,
		guneyDogu,
		guneyBati;

	string girisDosyasininAdi = argv[1], cikisDosyasininAdi, linuxKomutu,durum="";

	int* girisMatrisi = nullptr,
		** islenecekElemanMatrisi = nullptr,
		* gonderilecekMatris = nullptr,
		* sonMatris = nullptr,
		*cikisMatrisi = nullptr,
		*sendCounts=nullptr,
		*displs=nullptr,
		*receiveBuffer=nullptr,
		*results=nullptr;

	size_t sonBoluIndisi = 0, 
		   sonNoktaIndisi = 0;


	ifstream girisDosyasi;
	ofstream cikisDosyasi;

	MPI_Init(&argc, &argv);

	baslangic = MPI_Wtime();

	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	if (myRank == 0) {

		girisDosyasi.open(girisDosyasininAdi, ios::in);
		girisDosyasi >> satir >> sutun;
		elemanSayisi = satir * sutun;

		girisMatrisi = new int[elemanSayisi];
		cikisMatrisi = new int[elemanSayisi];

		for (int i = 0; i < elemanSayisi; i++) {
			girisDosyasi >> girisMatrisi[i];
		}

		for (int i = 0; i < satir; i++) {
			for (int j = 0; j < sutun; j++) {
				diziIndisi = i * sutun + j;
				if (Sinir_Durum_Kontrol_Fonksiyonu(satir, sutun, i, j)) {
					islenecekElemanSayisi++;
				}
				else {
					cikisMatrisi[diziIndisi] = girisMatrisi[diziIndisi];
				}
			}
		}

		if (islenecekElemanSayisi % size != 0) {
			girisDosyasi.close();
			delete[] cikisMatrisi;
			cout << islenecekElemanSayisi << endl;
			durum += "[";
			for (int i = 1; i < islenecekElemanSayisi; i++) {
				if (islenecekElemanSayisi % i == 0) {
					durum += to_string(i) + ",";
				}
			}
			durum += to_string(islenecekElemanSayisi) + "]";

			cout << "Blok Dagitim Teknigi (arraySize%commSize==0) uygulanamiyor. Lutfen hesaplama dugumlerinin sayisi " << durum  << " sayi kumesinden biri olsun." << endl;

			MPI_Abort(MPI_COMM_WORLD, 99);
		}

		islenecekElemanMatrisi = new int* [islenecekElemanSayisi];
		gonderilecekMatris = new int[islenecekElemanSayisi * 10];

		for (int i = 0; i < islenecekElemanSayisi; i++) {
			islenecekElemanMatrisi[i] = new int[10]; // 9 komþu + indis
		}

		counter = 0;

		for (int i = 0; i < satir; i++) {
			for (int j = 0; j < sutun; j++) {
				diziIndisi = i * sutun + j;
				if (Sinir_Durum_Kontrol_Fonksiyonu(satir, sutun, i, j)) {

					kuzeyBati = girisMatrisi[(i - 1) * sutun + (j - 1)];				// Kuzey Bati Komsu
					kuzey = girisMatrisi[(i - 1) * sutun + j];							// Kuzey Komsu
					kuzeyDogu = girisMatrisi[(i - 1) * sutun + (j + 1)];				// Kuzey Dogu Komsu
					bati = girisMatrisi[i * sutun + (j - 1)];							// Bati Komsu
					merkez = girisMatrisi[i * sutun + j];								// Cekirdegin Merkezi
					dogu = girisMatrisi[i * sutun + (j + 1)];							// Dogu Komsu
					guneyBati = girisMatrisi[(i + 1) * sutun + (j - 1)];				// Guney Bati Komsu
					guney = girisMatrisi[(i + 1) * sutun + j];							// Guney Komsu
					guneyDogu = girisMatrisi[(i + 1) * sutun + (j + 1)];				// Guney Dogu Komsu

					islenecekElemanMatrisi[counter][0] = kuzeyBati;
					islenecekElemanMatrisi[counter][1] = kuzey;
					islenecekElemanMatrisi[counter][2] = kuzeyDogu;
					islenecekElemanMatrisi[counter][3] = bati;
					islenecekElemanMatrisi[counter][4] = merkez;
					islenecekElemanMatrisi[counter][5] = dogu;
					islenecekElemanMatrisi[counter][6] = guneyBati;
					islenecekElemanMatrisi[counter][7] = guney;
					islenecekElemanMatrisi[counter][8] = guneyDogu;
					islenecekElemanMatrisi[counter][9] = diziIndisi;

					counter++;
				}
			}
		}

		int indis_ = 0;

		for (int i = 0; i < islenecekElemanSayisi; i++) {

			kuzeyBati = islenecekElemanMatrisi[i][0];
			kuzey = islenecekElemanMatrisi[i][1];
			kuzeyDogu = islenecekElemanMatrisi[i][2];
			bati = islenecekElemanMatrisi[i][3];
			merkez = islenecekElemanMatrisi[i][4];
			dogu = islenecekElemanMatrisi[i][5];
			guneyBati = islenecekElemanMatrisi[i][6];
			guney = islenecekElemanMatrisi[i][7];
			guneyDogu = islenecekElemanMatrisi[i][8];
			indis_ = islenecekElemanMatrisi[i][9];

			diziIndisi = i * 10;

			gonderilecekMatris[diziIndisi + 0] = kuzeyBati;
			gonderilecekMatris[diziIndisi + 1] = kuzey;
			gonderilecekMatris[diziIndisi + 2] = kuzeyDogu;
			gonderilecekMatris[diziIndisi + 3] = bati;
			gonderilecekMatris[diziIndisi + 4] = merkez;
			gonderilecekMatris[diziIndisi + 5] = dogu;
			gonderilecekMatris[diziIndisi + 6] = guneyBati;
			gonderilecekMatris[diziIndisi + 7] = guney;
			gonderilecekMatris[diziIndisi + 8] = guneyDogu;
			gonderilecekMatris[diziIndisi + 9] = indis_;

		}
		
	

		sonMatris = new int[islenecekElemanSayisi*2];
	}

	MPI_Bcast(&islenecekElemanSayisi, 1, MPI_INT, 0, MPI_COMM_WORLD);

	alimBoyutu = (islenecekElemanSayisi / size) * 10;
	receiveBuffer = new int[alimBoyutu];
	results = new int[(islenecekElemanSayisi / size)*2];
	
	MPI_Scatter(gonderilecekMatris, alimBoyutu, MPI_INT, receiveBuffer, alimBoyutu, MPI_INT, 0, MPI_COMM_WORLD);

	counter = 0;
	for (int i = 0; i < alimBoyutu; i += 10) {
		yeniPixelDegeri =
			filtreMatrisi[0][0] * receiveBuffer[i + 0] +
			filtreMatrisi[0][1] * receiveBuffer[i + 1] +
			filtreMatrisi[0][2] * receiveBuffer[i + 2] +
			filtreMatrisi[0][3] * receiveBuffer[i + 3] +
			filtreMatrisi[0][4] * receiveBuffer[i + 4] +
			filtreMatrisi[0][5] * receiveBuffer[i + 5] +
			filtreMatrisi[0][6] * receiveBuffer[i + 6] +
			filtreMatrisi[0][7] * receiveBuffer[i + 7] +
			filtreMatrisi[0][8] * receiveBuffer[i + 8] ;

		if (yeniPixelDegeri < 0) {
			yeniPixelDegeri = 0;
		}
		else if (yeniPixelDegeri > 255) {
			yeniPixelDegeri = 255;
		}

		results[counter++] = receiveBuffer[i + 9];
		results[counter++] = yeniPixelDegeri;

	}
	
	MPI_Gather(results, counter, MPI_INT, sonMatris, counter, MPI_INT, 0, MPI_COMM_WORLD);

	if (myRank == 0) {

		for (int i = 0; i < islenecekElemanSayisi*2; i+=2) {
			cikisMatrisi[sonMatris[i]] = sonMatris[i+1];
		}

		cikisDosyasininAdi = girisDosyasininAdi;
		sonBoluIndisi = cikisDosyasininAdi.rfind("/");
		sonNoktaIndisi = cikisDosyasininAdi.rfind(".");
		cikisDosyasininAdi = cikisDosyasininAdi.substr(sonBoluIndisi + 1, sonNoktaIndisi - sonBoluIndisi - 1);
		cikisDosyasininAdi += "_Filtered.txt";

		cikisDosyasi.open(cikisDosyasininAdi.c_str(), ios::out);

		
		for (int i = 0; i < satir; i++) {
			for (int j = 0; j < sutun; j++)
			{
				diziIndisi = i * sutun + j;
				cikisDosyasi << cikisMatrisi[diziIndisi];

				if (j == (sutun - 1)) {
					cikisDosyasi << "\n";
				}
				else {
					cikisDosyasi << "\t";
				}
			}
		}

		linuxKomutu = "./ConversionScript.sh " + cikisDosyasininAdi;
		if (system(linuxKomutu.c_str()) != -1) {
			cout << "Filtrelenmis Goruntu Matrisi Basari Ile Goruntu Dosyasina Donusturuldu.\n";
		}
		else {
			cout << "Linux Scripti Calistirilamadi.\n";
		}
		
		
		
		for (int i = 0; i < islenecekElemanSayisi; i++) {
			delete[] islenecekElemanMatrisi[i];
		}
		delete[] sonMatris;
		delete[] girisMatrisi;
		delete[] gonderilecekMatris;
		delete[] islenecekElemanMatrisi;
		delete[] cikisMatrisi;
		girisDosyasi.close();
		cikisDosyasi.close();
	}
	

	delete[] results;
	delete[] receiveBuffer;

	bitis = MPI_Wtime();

	MPI_Finalize();

	if (myRank == 0) {
		cout << "Gecen toplam sure : " << setprecision(5) << bitis - baslangic << " sn" << endl;
	}

	return 0;
}
