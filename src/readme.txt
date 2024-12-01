Here is new pin configuration:
pin configuration for led strip:
- Ready to use: A6, A7, A8, A9, A10, A11, A0, A1, B6, B7, B8, B9, B0, B1
    A1 -> B3
    A4, A5, A6, A7: SD card  -> B12, B13, B14, B15
    B0, B1: Vr, Sound sensor -> A4, A5


- A2, A3-> no change
===========================================================================================================
021224: Fixed bug run  only 4 ports.
Compare với phần code test stub cũ thì chỉ khác phần TIM config và clock.
Trace ngược lại thì có đoạn clock division chỗ RCC_Configuration().
Bài học rút ra: cứ compare, thấy khác nhau ở đâu cứ xoáy vào chỗ đó. Làm cho tương tự bản cũ là ngon.
===========================================================================================================