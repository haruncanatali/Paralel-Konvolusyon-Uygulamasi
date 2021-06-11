# Paralel Konvolusyon İşlemi
Bu projede MPI kütüphanesi kullanılarak C++ dilinde paralel olarak örnek bir fotoğrafa konvolusyon(filtreleme) işlemi yapılmıştır.

#### Yöntem<hr>
![indir](https://user-images.githubusercontent.com/77530565/121698206-72a28e00-cad6-11eb-971c-55fd1b3c6bba.png)
* Bu filtre maskesi ilk satır,ilk sutun,son satır,son sutun haricindeki bütün değerlere işlem yapacaktır. Bu projede Emboss Filtre Çekirdeği kullanılacaktır.
* Proje linux ortamında çalıştırılmıştır. Kullanıcıdan **parametre** olarak kaynak görüntü matrisinin dosya yolunu alır ve çıkış matrisi üretir. Bu çıkış matrisi bir Script aracılığıyla görüntü dosyasına dönüştürülür. 
*  Script projenin bulunduğu dizinde sonuç matrisini ve sonuç görüntü dosyasını oluşturur.
* Kolaylık sağlansın diye GoruntuMatrisi1.txt dosyasının ilk satırındaki iki değer, matrisin satır ve sütun sayısıdır. 

#### Proje Çıktısı <hr>
![Tasarım](https://user-images.githubusercontent.com/77530565/121706318-3115e100-cade-11eb-8a4f-83f6a8afb905.png)