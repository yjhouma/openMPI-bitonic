### Petunjuk
    Jalankan `make` pada terminal
    Jalankan `mpirun -np <num_proc> ./bitonic_sort <arr_length>`

    num_proc adalah jumlah thread yang diinginkan, serta arr_length adalah jumlah array 
    yang diinginkan.

### Jumlah thread
Jumlah thread merupakan variable untuk program ini. Disarankan jumlah thread yang diberikan
tidak melebihi jumlah core pada processor komputer

### Penjelasan Solusi
Untuk contoh, dimisalkan ada x proses dan panjang k. Pada awalnya, array berada pada proses 0. State ini adalah merupakan node process tree dengan kedalaman 0 atau
merupakan root. Untuk setiap root dengan kedalaman log2(x) maka sort secara serial. Untuk node dengan 
kedalaman kurang dari log2(x) maka dibagi menjadi 2 bagian yaitu bagian awal dan akhir. Separuh pertama
disimpan untuk proses itu misal dinamai A. Separuh kedua dari array dikirimkan pada proses A+x(2^(b+1))
dimana b adalah kedalaman A. Setelah sorting selesai, penggabungan terjadi dan disimpan di proses A.

### Analisis 
Setelah melakukan eksperimen dengan menggunakan program ini secara paralel dan serial. Bisa dilihat bahwa waktu berjalan lebih cepat saat dijalankan secara paralel. Hal ini dikarenakan proses dibagikan
menjadi beberapa thread dan bekerja secara paralel



### Hasil pengukuran kerja

#### 5,000
    #1
    Serial	: 4272 microseconds
    Paralel	: 1268  microseconds

    #2
    Serial	: 3632 microseconds
    Paralel	: 907 microseconds

    #3
    Serial	: 3451 microseconds
    Paralel	: 965 microseconds


#### 50,000

    #1
    Serial	: 34953 microseconds
    Paralel	: 9828 microseconds

    #2
    Serial	: 33947 microseconds
    Paralel	: 21093 microseconds

    #3
    Serial	: 35661 microseconds
    Paralel	: 25324 microseconds


#### 100,000

    #1
    Serial	: 78044 microseconds
    Paralel	: 31812 microseconds

    #2
    Serial	: 83517 microseconds
    Paralel	: 51449 microseconds

    #3
    Serial	: 76137 microseconds
    Paralel	: 57430 microseconds


#### 200,000

    #1
    Serial	: 180421 microseconds
    Paralel	: 100388 microseconds

    #2
    Serial	: 171528 microseconds
    Paralel	: 76276 microseconds

    #3
    Serial	: 189377 microseconds
    Paralel	: 69206 microseconds

#### 400.000

    #1
    Serial	: 180421 microseconds
    Paralel	: 100388 microseconds

    #2
    Serial	: 171528 microseconds
    Paralel	: 76276 microseconds

    #3
    Serial	: 189377 microseconds
    Paralel	: 69206 microseconds

