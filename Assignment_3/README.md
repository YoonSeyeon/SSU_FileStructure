# Flash Memory에서의 Block Mapping FTL 구현   
### <Flash Memory 계층 구조>
<img src="Picture/Flash Memory 계층 구조.jpg" width="30%" height="30%"></img><br/>   

### <Block Mapping 기법>
<img src="Picture/Block Mapping.jpg" width="80%" height="80%"></img><br/>
- - -

## Block Mapping 기법을 따르는 FTL 구현 
* blockmap.h : FTL 구현에 쓰이는 상수들을 선언
* fdevicedriver.c : Flash Device Driver는 메모리에 직접 접근하여 데이터를 Read / Write
* ftl.c : FTL은 논리주소를 물리주소로 mapping
* main.c : 테스트 프로그램 작성