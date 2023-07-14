#include <fcntl.h>
#include <stdio.h>//NAO PODE USAR,SO ESTOU USANDO PARA AUXILIAR NO DESENVOLVIMENTO
#include <unistd.h>

typedef struct {
	char st_name[8];
	char st_value[8];
	char st_size[8];
	unsigned char	st_info;
	unsigned char	st_other;
	char st_shndx[4];
} Elf32_Sym;

typedef struct{

	char nome[50];
	char vma[50];
	char size[50];
	int address_dec;

}struct_h;

typedef struct 
{
	char address[50];
	char flag;
	char section[50];
	char other_info[50];
	char nome[50];
	int adress_dec;

}struct_t;


const unsigned int MAX = 1000*100;

//funcoes para converter,traduzir,pegar data...

char* dec_hex(int value,char *result);

char* hex_ascii(char *hex,int size,char* result);

int hex_dec_little(char *hex_little,int size);

int get_hex_data(char* buffer,int strt_indx,int end_indx,char* result);

void little_big_endian(char *hex_little,int size);


//funcoes para lidar com strings

void clean_buffer(char *buffer,int size);

void copy_str(char *str1,char *str2,int size);

int equals(char *str1,char *str2);

int str_len(char* str);

int main(){

	int fd = open("test-00.x", O_RDONLY);

	if(fd != -1){
		char buffer[MAX];
		read(fd,buffer,MAX);


		//1)e_shoff,e_shnum,e_shstrnd
		
		char e_shoff_hex[20];

		get_hex_data(buffer,32,32+4,e_shoff_hex);

		int e_shoff = hex_dec_little(e_shoff_hex,8);
		//printf("sh comecam em : %d\n",e_shoff);

		char e_shnum_hex[20];
		char e_shstrndx_hex[20];

		get_hex_data(buffer,48,48+2,e_shnum_hex);
		get_hex_data(buffer,48+2,48+4,e_shstrndx_hex);


		int e_shnum =  hex_dec_little(e_shnum_hex,4);
		//printf("quantidade de sections headers : %d\n",e_shnum);
		int e_shstrndx = hex_dec_little(e_shstrndx_hex,4);
		//printf("index da section header str table : %d\n",e_shstrndx);


		//2) achar o offsets da str table

		//sh_offset:0,1,2,3
		//size:23,22,21,20
		//offset(so importa para o strt tab):19,18,17,16
		//VMA:15,14,13,12

	
		char str_off_hex[20];
		int strt =  16+(e_shoff)+(e_shstrndx*40);
		get_hex_data(buffer,strt,strt+4,str_off_hex);
		int str_off = hex_dec_little(str_off_hex,8);
		//printf("%d\n",str_off);

		//para cada secao,procurar os dados e o nome
		//o nome e so usar o offset dessa secao mesmo

		//3)Iterando nas section headers e pegando os dados; -h do objdump

		struct_h sections_data[100];
		int symtab_offset;
		int symtab_size;
		int strtab_offset;
		for(int i = 0;i < e_shnum;i++){
			
			struct_h section;

			int index = i;

			int strt_indx = e_shoff+(i*40);

			//1)achando o nome:

			//1.1)sh_offset

			char offset_hex[20];
			get_hex_data(buffer,strt_indx,strt_indx+4,offset_hex);
			int offset = hex_dec_little(offset_hex,8);
			//printf("section offset : %d\n",section.offset);
			
			//1.2)achar o nome 

			char nome_hex[50];
			char nome[50];
			int size = get_hex_data(buffer,str_off+offset,-1,nome_hex);
			hex_ascii(nome_hex,size,nome);
			//printf("nome : %s\n",nome);
			copy_str(nome,section.nome,size);
			//printf("nome : %s\n",section.nome);
			clean_buffer(nome,size);
			if(equals(section.nome,".strtab")){
				strtab_offset = str_off+offset+size;
			}
			if(equals(section.nome,".symtab")){
				char symtab_offset_hex[50];
				size = get_hex_data(buffer,strt_indx+16,strt_indx+16+4,symtab_offset_hex);
				symtab_offset = hex_dec_little(symtab_offset_hex,size);

			}

			//1)
			
			//2)achando o size

			char size_hex[50];
			size = get_hex_data(buffer,strt_indx+20,strt_indx+20+4,size_hex);
			if(equals(section.nome,".symtab")){
				symtab_size = hex_dec_little(size_hex,size);
			}
			little_big_endian(size_hex,size);
			//printf("size : %s\n",size_hex);
			copy_str(size_hex,section.size,size);
			//printf("size : %s\n",section.size);

			//2)

			//3)achando o VMA

			char vma_hex[50];
			size = get_hex_data(buffer,strt_indx+12,strt_indx+12+4,vma_hex);
			section.address_dec = hex_dec_little(vma_hex,size);
			little_big_endian(vma_hex,size);
			//printf("vma : %s\n",vma_hex);
			copy_str(vma_hex,section.vma,size);
			//printf("vma : %s\n",section.vma);



			sections_data[i] = section;


		}

		//printar os dados da array sections_data:
		//RESPOSTA DO COMANDO -h
		printf("Idx Name Size VMA\n");
		for(int i = 0;i < e_shnum;i++){
			struct_h x = sections_data[i];
			printf("%d %s %s %s\n",i,x.nome,x.size,x.vma);

		}
		

		//SYMBOL TABLE(-t):
		//procurar o index da symtab,o tamanho e so traduzir
		//printf("symtab starta aqui : %d\n",symtab_offset);
		//printf("symtab size : %d\n",symtab_size);

		//nesse caso: symtab vai de 252 ate 252+96,sao 6 linhas com 16 bytes mas na pratica so os 8 primeiros importam
		//dai procurar os index na strtab 4 primeiros,oque sera guardado e do 5,7,8,9:
		int symtab_offsets[50];//guarda os offsets dos simbolos na strtab(os 4 primeiros bytes de cada linha)
		struct_t symbtab_data[50];
		
		//conseguir os offsets dos rotulos na strtab e preencher os campos address e other_info

		int num_symb = symtab_size/16;
		for(int i = 0;i < num_symb;i++){

			int strt_indx = symtab_offset+i*16;
			struct_t symb;

			//offsets:
			char offset_hex[50];
			int size = get_hex_data(buffer,strt_indx,strt_indx+4,offset_hex);
			symtab_offsets[i] = hex_dec_little(offset_hex,size);
			//printf("offset : %d\n",symtab_offsets[i]);

			//address:
			char address_hex[50];
			size = get_hex_data(buffer,strt_indx+4,strt_indx+4+4,address_hex);
			symb.adress_dec = hex_dec_little(address_hex,size);
			little_big_endian(address_hex,size);
			copy_str(address_hex,symb.address,size);
			//printf("address : %s\n",symb.address);

			//other_info
			char info_hex[50];
			size = get_hex_data(buffer,strt_indx+8,strt_indx+8+4,info_hex);
			little_big_endian(info_hex,size);
			copy_str(info_hex,symb.other_info,size);
			//printf("other_info : %s\n",symb.other_info);

			symbtab_data[i] = symb;

		}

		//procurar o index logo apos a strtab
		//printf("strtab_offset : %d\n",strtab_offset);

		//completar os campos nome e flag

		int global = 0;
		for(int i = 0;i < num_symb;i++){

			int strt_indx = strtab_offset+symtab_offsets[i]+1;

			//nome:
			char nome_hex[50];
			int size = get_hex_data(buffer,strt_indx,-1,nome_hex);
			char nome[50];
			hex_ascii(nome_hex,size,nome);
			copy_str(nome,symbtab_data[i].nome,size);
			//printf("nome : %s\n",nome_hex);
			//printf("nome : %s\n",symbtab_data[i].nome);

			clean_buffer(nome,size*2);

			if(equals(symbtab_data[i].nome,"_start")){
				global = 1;
			}

			//flag:
			if(!global){
				symbtab_data[i].flag = 'l';
			}
			else{
				symbtab_data[i].flag = 'g';
			}

		}

		//achar em qual secao o symb esta

		int text_address;
		for(int i = 0;i < num_symb;i++){

			struct_t symb = symbtab_data[i];

			int has_section = 0;
			for(int j = 0;j < e_shnum;j++){

				struct_h section = sections_data[j];

				if(equals(section.nome,".text")){
					text_address = section.address_dec;
				}

				if(symb.adress_dec >= section.address_dec && section.address_dec >= text_address){
					int len = str_len(section.nome);
					copy_str(section.nome,symbtab_data[i].section,len);
					has_section = 1;
				}

			}
			if(!has_section){
				copy_str("*ABS*",symbtab_data[i].section,5);
			}
			//printf("-->%s\n",symbtab_data[i].section);


		}

		//printando -t:

		printf("SYMBOL TABLE:\n");
		for(int i = 0;i < num_symb;i++){

			struct_t symb = symbtab_data[i];

			if(equals(symb.address,"00000000")){
				continue;
			}

			printf("%s %c %s %s %s\n",symb.address,symb.flag,symb.section,symb.other_info,symb.nome);

		}

		//agora o -d,nao sei nem por onde comecar bora pesquisar ai...
	
		




	}

	

    return 0;
}

char convert_unit(int unit){

	if(unit < 10){
		return unit+'0';
	}
	else{

		if(unit == 10){
			return 'a';
		}
		if(unit == 11){
			return 'b';
		}
		if(unit == 12){
			return 'c';
		}
		if(unit == 13){
			return 'd';
		}
		if(unit == 14){
			return 'e';
		}
		if(unit == 15){
			return 'f';
		}

	}
	char invalid = '!';
	return invalid;	

}
char* dec_hex(int value,char* result){
	//eu sei que vou converter 1 byte entao sao 2 digitos hexadecimais
	
	if(value < 0){
		value = 255+value + 1;//255+value equivale a flipar os bits
	}
	result[0] = '0';
	result[1] = '0';
	int i = 1;
	while(value > 0){

		result[i] = convert_unit(value % 16);
		value /= 16;
		i--;
	}
	return result;

}

int poww(int base,int exp){

	int pow = 1;
	for(int i = 0;i < exp;i++){

		pow *= base;

	}
	return pow;
}

int decod_unit(char unit){

	if(unit-'0' < 10){
		return unit - '0';
	}
	else{

		if(unit == 'a'){
			return 10;
		}
		if(unit == 'b'){
			return 11;
		}
		if(unit == 'c'){
			return 12;
		}
		if(unit == 'd'){
			return 13;
		}
		if(unit == 'e'){
			return 14;
		}
		if(unit == 'f'){
			return 15;
		}
	}
	return -1;
}

char * hex_ascii(char *hex,int size,char *result){


	int c = 0;
	int i = 0;
	int curr = 0;
	int exp = 1;
	while(i < size*2){//multiplica por 2 pq cada caracter em ascii sao codificados em 1 byte(1 valor no buffer -> 2 digitos hex)
		curr += (decod_unit(hex[i]))*poww(16,exp);
		if((i+1) % 2 == 0){
			result[c] = (char)curr;
			//printf("%d --> %c\n",i,result[c]);
			exp = 1;
			c++;
			curr = 0;
			i++;
			continue;
		}
		i++;
		exp--;

	}
	result[c] = '\0';
	return result;

}

int hex_dec_little(char *hex_little,int size){
	//hex em little endian

	if(hex_little[0] == '0'){
		return decod_unit(hex_little[1]);
	}

	int curr = 0;
	for(int i = size-2;i >= 0;i -= 2){
		int a = decod_unit(hex_little[i]);
		int b = decod_unit(hex_little[i+1]);
		curr += a*poww(16,i+1) + b*poww(16,i);

	}
	return curr;
}

int get_hex_data(char* buffer,int strt_indx,int end_indx,char *result){

	/*Essa funcao pega um range de valores no buffer e traduz em uma string hexadecimal,para depois ser traduzida se preciso
	se end_indx = -1,eu vou considerar de strt_indx ate aparescer o primeiro 00,particularmente util para achar os nomes na tabela
	*/

	if(end_indx == -1){
		int i = strt_indx;
		while (buffer[i] != 0)
		{
			i++;
		}
		end_indx = i;
	}

	int c = 0;
	for(int i = strt_indx;i < end_indx;i++){

		char tmp[2];
		dec_hex(buffer[i],tmp);

		for(int j = 0;j < 2;j++){
			result[c] = tmp[j];
			c++;
		}

	}

	return end_indx - strt_indx;
}

void clean_buffer(char *buffer,int size){

	for(int i = 0;i < size;i++){

		char tmp = '\0';
		buffer[i] = tmp;

	}

}

void swap(char *a,char *b){
	char tmp = *a;
	*a = *b;
	*b = tmp;
}

void little_big_endian(char *hex_little,int size){

	//dado um hex em little endian passar para big endian
	//ab cd --> cd ab ; 08 00 --> 00 08
	size *= 2;//cada digito do buffer sao 2 digitos hex
	int strt = 0;
	int end = size-1;

	while (strt <= end)
	{
		swap(&hex_little[strt],&hex_little[end]);
		strt++;
		end--;
	}
	
	for(int i = 1;i < size;i += 2){
		swap(&hex_little[i],&hex_little[i-1]);
	}

}

void copy_str(char *str1,char *str2,int size){

	//copia o counteudo de 1 em 2
	size *= 2;
	for(int i = 0;i < size;i++){

		str2[i] = str1[i];

	}

}

int equals(char *str1,char *str2){

	int i = 0;
	while (str1[i] != '\0')
	{
		if(str2[i] == '\0'){
			return 0;
		}
		if(str1[i] != str2[i]){
			return 0;
		}
		i++;
	}
	if(str1[i] != str2[i]){
		return 0;
	}

	return 1;
	
}

int str_len(char* str){

	int i = 0;
	while (str[i] != '\0')
	{
		i++;
	}
	return i;

}
