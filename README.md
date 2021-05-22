# FFT-Noise
Utilizing 2D Fast Fourier Transforms on 2D vectors/ images to generate 1/f Noise.

## How to Compile & Run
<details>
  <summary><b> Linux & Mac </b> </summary>
  
  ```shell
  git clone https://github.com/IamShubhamGupto/FFT-Noise.git
  cd FFT-Noise
  make 
  ./bin/fft.out beta
  ```  

  <b>EXAMPLE </b>
  
  ```shell
  make 
  ./bin/fft.out 1.7
  ```
  
  Guide:
  
  ```
Usage: ./bin/fft.out mode path-to-input-image beta

beta - roughness factor
  ```
  #### Clean Up
  In order to clean the bin directory, run ```make clean```.

  In order to remove only the generated images, run ``` make cleanimg```.
</details> 

## Results

