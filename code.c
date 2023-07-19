#include <fcntl.h>
#include <stdio.h>//NAO PODE USAR,SO ESTOU USANDO PARA AUXILIAR NO DESENVOLVIMENTO
#include <unistd.h>


char instruction_type[100][50] = {

	//X_N -> tipo da instrucao,baseado nas divisoes da tabela do livro
	//-xxxxxxx(7bits)->bits que indicam o tipo da instrucao

	"U_0-0110111",
	"U_1-0010111",
	"J_0-1101111",
	"I_0-1100111",
	"B_0-1100011",
	"I_1-0000011",
	"S_0-0100011",
	"I_2-0010011",
	"R_0-0110011",
	"I_3-0001111",
	"I_4-1110011",
	"."
};


char registor_table[][10] = {

	"zero",//0
	"ra",//1
	"sp",//2
	"gp",
	"tp",
	"t0",
	"t1",
	"t2",
	"s0",
	"s1",
	"a0",
	"a1",
	"a2",
	"a3",
	"a4",
	"a5",
	"a6",
	"s2",
	"s3",
	"s4",
	"s5",
	"s6",
	"s7",
	"s8",
	"s9",
	"s10",
	"s11",
	"t3",
	"t4",
	"t5",
	"t6"//31
};
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

typedef struct
{
	char adress[50];
	char bytes[50];
	char normal_bytes[50];
}address_bytes;

typedef struct
{
	char nome[50];
	address_bytes list[100];
	int size;

}struct_d;



const unsigned int MAX = 1000*100;

//funcoes para converter,traduzir,pegar data...

char* dec_hex(int value,char *result);

char* hex_ascii(char *hex,int size,char* result);

int hex_dec_little(char *hex_little,int size);

int get_hex_data(char* buffer,int strt_indx,int end_indx,char* result);

void little_big_endian(char *hex_little,int size);

void inc_4(char *hex,int len);

int address_of_symb(char *address,struct_t *symb_data,int num_symb,char *nome);

int readable_bytes(char *bytes,int size,char *readable);

void get_rid_zeros(char *address,char *copy);

int no_spaces(char* hex,char* result,int size);

int hex_to_bin(char *hex,char *bin);

int get_type(char* bits,char *type);

int get_instructions(char* type,char *bits,char *instruction);

int get_nome(char *type,char *func3,char *nome);

int get_bits(char *bits,int strt,int end,char *data);

int bin_dec(char *bin);

int decod_instruction(char *type,char *bits,char* decod);

int bin_dec_little(char *bin);

int bin_little_endian(char *hex,char *bits);

void print_decod(char *decod,int len);

int decod_B_0(char *bits,char *decod,char *address,char *symb);

//funcoes para lidar com strings

void clean_buffer(char *buffer,int size);

void copy_str(char *str1,char *str2,int size);

int equals(char *str1,char *str2);

int str_len(char* str);

int get_string(char* realtion,char* string);

int int_to_string(int value,char *string);

void str_reverse(char *str,int end,int strt);

void str_cpy(char *str1,char *str2);

void append(char* str1,char *str2,int num);

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
		int text_offset;
		int text_size;
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
			if(equals(section.nome,".text")){
				text_size = hex_dec_little(size_hex,size);
			}
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

			//offset na file image
			if(equals(section.nome,".text")){
				char inst_hex[50];
				size = get_hex_data(buffer,strt_indx+16,strt_indx+16+4,inst_hex);
				text_offset = hex_dec_little(inst_hex,size);
			}
			//printf("offset : %d\n",offset_inst);


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

		//printf("instrucoes da text start : %d\n",text_offset);
		//printf("size da text  : %d\n",text_size);
		//procurar os simbolos que sao da text e achar seus enderecos,comeca pelo _start e vai checando se tal endereco e de outro simbolo
		//o campo size diz quantos bytes sao no total para achar as instrucoes

		int strt_h;
		char hex_strt[50];
		char symb_address[50];
		for(int i = 0;i < num_symb;i++){
			if(equals(symbtab_data[i].nome,"_start")){
				strt_h = symbtab_data[i].adress_dec;
				int len = str_len(symbtab_data[i].address);
				copy_str(symbtab_data[i].address,hex_strt,len);
				copy_str(symbtab_data[i].address,symb_address,len);

			}
		}

		struct_d d_list[20];

		int num_h = text_size/4;
	    

		int c = 0;
		int k = 0;
		char curr_nome[50];
		int len_curr = address_of_symb(hex_strt,symbtab_data,num_symb,curr_nome);
		for(int i = 0;i <= num_h;i++){

			//
			char nome[50];
			int len = address_of_symb(hex_strt,symbtab_data,num_symb,nome);
			int len_strt = str_len(hex_strt);
			if((i != 0 && len != -1) || i == num_h){

				copy_str(curr_nome,d_list[c].nome,len_curr);
				//printf("curr_nome : %s\n",d_list[c].nome);							
				clean_buffer(curr_nome,len_curr);
				copy_str(nome,curr_nome,len);
				d_list[c].size = k;
				c++;
				k = 0;


			}
			//
			address_bytes tmp;
			copy_str(hex_strt,tmp.adress,len_strt);
			
			inc_4(hex_strt,len_strt);

			char bytes[50];
			int strt = text_offset+i*4;
			int size = get_hex_data(buffer,strt,strt+4,bytes);
			bytes[size*2] = '\0';//esqueci de fazer isso no get_hex_data,se houver alguma escrotidao ai pode ser isso...
			/*char bits[100];
			size = hex_to_bin(bytes,bits);
			copy_str(bits,tmp.bits,size);*/
			char bytes_read[50];
			str_cpy(bytes,tmp.normal_bytes);
			size = readable_bytes(bytes,size*2,bytes_read);
			copy_str(bytes_read,tmp.bytes,size);

			d_list[c].list[k] = tmp;
			k++;

		}


		//testando os enderecos
		printf("DISASSEMBLER,FINALMENTE!!!!!!!!!!!!!!\n");
		for(int i = 0;i < c;i++){

			struct_d symb_d = d_list[i];
			int size = symb_d.size;

			printf("%s <%s>\n",symb_d.list[0].adress,symb_d.nome);
			int first = 1;
			char first_cpy[50];
			for(int j = 0;j < size;j++){

				char copy[50];
				get_rid_zeros(symb_d.list[j].adress,copy);
				if(first){
					str_cpy(copy,first_cpy);
					first = 0;
				}
				printf("%s : %s ",copy,symb_d.list[j].bytes);

				//bits em little endian
				char bits[100];
				bin_little_endian(symb_d.list[j].normal_bytes,bits);
				//printf("bits : %s\n",bits);
				
				//achar o tipo da instrucao(func_type)
				char func_type[20];
				get_bits(bits,0,6,func_type);
				str_reverse(func_type,0,6);
				char type[10];
				get_type(func_type,type);
				char decod[MAX];
				int len = 0;
				if(equals(type,"B_0")){
					len = decod_B_0(bits,decod,first_cpy,symb_d.nome);
				}
				else{
					len = decod_instruction(type,bits,decod);
				}
				print_decod(decod,len);
				printf("\n");
				



			}


		}

		//testar a funcoes que pegam as instrucoes 

		//1) dado 7 bits achar o tipo

		/*char bits[] = "1100011";
		char type[10];
		get_type(bits,type);
		printf("type : %s\n",type);*/

		//2)dado o tipo e 3 bits achar a instrucao
		
		/*char func3[] = "000";
		char nome[20];
		get_nome(type,func3,nome);
		printf("nome : %s\n",nome);*/

		//depois,comecar a decodificar os tipos U_0,U_1,J_0,B_0

		//teste bin_dec_little:
		/*char bin[] = "0110111111111111";
		printf("bin_dec_little : %d\n",bin_dec_little(bin));*/

		//teste U_0: n achei lui

		//teste U_1:
		/*char bits[] = "10010111000000000000000000000000";//little endian
		char decod[MAX];
		int len = decod_instruction("U_1",bits,decod);

		printf("decod:\n");
		for(int i = 0;i < len;i++){
			printf("%s",decod+50*i);
		}*/

		//teste I_0 :
		/*char bits[] = "11100111000000010000000000000000";
		char decod[MAX];
		int len = decod_instruction("I_0",bits,decod);
		printf("decod:\n");
		for(int i = 0;i < len;i++){
			printf("%s",decod+50*i);
		}*/

		//teste I_1
		/*char hex[] = "035504ff";
		char bits[50];
		bin_little_endian(hex,bits);
		printf("bits : %s\n",bits);
		char decod[MAX];
		int len = decod_instruction("I_1",bits,decod);
		print_decod(decod,len);*/

		//teste I_2
		/*char hex[] = "130101fb";
		char bits[50];
		bin_little_endian(hex,bits);
		printf("bits : %s\n",bits);
		char decod[MAX];
		int len = decod_instruction("I_2",bits,decod);
		print_decod(decod,len);*/

		//continuando o teste do I_2
		//SUSPEITO QUE O PROFESSOR TROCOU o xori e o slli na hora de identificar os mnemonicos
		//xori tem func3 = 100 e slli tem func3 = 001,mas suponho que o professor inverteu,se esse problema se repetir em outro casos nao relacionados
		//o problema e do meu codigo mesmo,se nao o problema e do codigo do professor para corrigir...
		/*char hex[] = "13c52302";
		char bits[50];
		bin_little_endian(hex,bits);
		printf("bits : %s\n",bits);
		char decod[MAX];
		int len = decod_instruction("I_2",bits,decod);
		print_decod(decod,len);*/

		//faltam : J,R,B...

		//teste append

		/*char str1[] = "abc";
		char str2[] = "xyzw";
		append(str1,str2,100);
		printf("str : %s\n",str1);*/

		//teste imm_S

		/*char hex[50] = "2320a4fe";
		char bits[50];
		bin_little_endian(hex,bits);
		char decod[MAX];
		int len = decod_instruction("S_0",bits,decod);
		print_decod(decod,len);*/

		//teste I_4

		/*char hex[50] = "f3576600";
		char bits[50];
		bin_little_endian(hex,bits);
		printf("bits : %s\n",bits);
		char decod[MAX];
		int len = decod_instruction("I_4",bits,decod);
		print_decod(decod,len);*/



		//teste B_0
		/*char hex[50] = "e31eb5fe";
		char bits[50];
		bin_little_endian(hex,bits);
		char decod[MAX];
		char address[20] = "110d4";
		char symb[20] = "<_start>";
		int len = decod_B_0(bits,decod,address,symb);
		print_decod(decod,len);*/

		//teste R_0

		/*char hex[50] = "3305b500";
		char bits[50];
		bin_little_endian(hex,bits);
		char decod[MAX];
		int len = decod_instruction("R_0",bits,decod);
		print_decod(decod,len);*/

		/*char hex[] = "97000000";
		char bits[50];
		bin_little_endian(hex,bits);
		printf("bits : %s\n",bits);*/






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

void inc_4(char *hex,int len){

	int end = len-1;

	int overflow = 4;
	for(int i = end;i >= 0;i--){

		int x = decod_unit(hex[i]);
		if(x+overflow >= 16){

			hex[i] = '0';
			overflow = x+overflow - 15;

		}
		else{
			hex[i] = convert_unit(x+overflow);
			overflow = 0;
			return;
		}

	}

}

int address_of_symb(char *address,struct_t *symb_data,int num_symb,char *nome){

	for(int i = 0;i < num_symb;i++){

		struct_t symb = symb_data[i];
		if(equals(address,symb.address)){
			
			int len = str_len(symb.nome);
			copy_str(symb.nome,nome,len);
			return len;

		}

	}
	return -1;

}

int readable_bytes(char *bytes,int size,char *readable){

	int spaces = (size/2)-1;


	int c = 0;
	int k = 0;
	for(int i = 0;i < size+spaces;i++){

		if(c == 2){
			readable[i] = ' ';
			c = 0;
			continue;
		}
		else{
			readable[i] = bytes[k];
			k++;
			c++;
		}

	}
	readable[size+spaces] = '\0';
	return size+spaces;

}

void get_rid_zeros(char *address,char *copy){



	while (address[0] != '\0')
	{
		if(address[0] == '0'){
			address++;
		}
		else{
			break;
		}

	}
	int i = 0;
	while (address[i] != '\0')
	{
		copy[i] = address[i];
		i++;
	}
	copy[i] = '\0';
	
}

int hex_to_bin(char *hex,char *bin){

	// 1 digito hex --> 4 digitos binarios
	int i = 0;
	int c = 0;
	while(hex[i] != '\0'){
		if(hex[i] == ' '){
			i++;
			continue;
		}
		int x = decod_unit(hex[i]);
		for(int j = 0;j < 4;j++){

			bin[c] = x%2 + '0';
			x /= 2;
			c++;

		}
		i++;


	}
	bin[c] = '\0';
	return c;

}

int no_spaces(char* hex,char* result,int size){

	int i = 0;
	int c = 0;
	while(i < size){
		if(hex[i] == ' '){
			i++;
			continue;
		}
		result[c] = hex[i];
		c++;
		i++;
	}
	result[c] = '\0';
	return c;

}

void str_cpy(char *str1,char *str2){

	int c = 0;
	while (str1[c] != '\0')
	{
		str2[c] = str1[c];
		c++;
	}
	str2[c] = '\0';

}

///

int get_string(char* relation,char* string){

	//pegar o counteudo pos -

	int i = 0;
	int can_store = 0;
	int c = 0;
	while (relation[i] != '\0')
	{
		if(relation[i] == '-'){
			can_store = 1;
			i++;
			continue;
		}
		if(can_store){
			string[c] = relation[i];
			c++;
		}
		i++;
	}
	return c;
}

int get_type_str(char* relation,char *type){

	//conteudo pre -

	int i = 0;
	while (relation[i] != '-')
	{
		type[i] = relation[i];
		i++;
	}
	type[i] = '\0';
	return i;
}

int get_type(char* bits,char *type){

	//dado 7 bits achar o tipo daquela instrucao

	int i = 0;
	while (!equals(instruction_type[i],"."))
	{
		char str[50];
		int len = get_string(instruction_type[i],str);
		//printf("str : %s\n",str);
		if(equals(str,bits)){
			return get_type_str(instruction_type[i],type);
		}
		i++;
	}
	return 0;
}


int U_0_nome(char *func3,char* nome){

	copy_str("lui",nome,3);
	return 3;

}

int U_1_nome(char *func3,char* nome){

	copy_str("aupic",nome,5);
	return 5;
}

int J_0_nome(char *func3,char* nome){

	copy_str("jal",nome,3);
	return 3;
}

int I_0_nome(char *func3,char* nome){

	copy_str("jalr",nome,4);
	return 4;
}

int I_1_nome(char *func3,char *nome){
	if(equals(func3,"000")){
		copy_str("lb",nome,2);
		return 2;
	}
	if(equals(func3,"001")){
		copy_str("lh",nome,2);
		return 2;
	}
	if(equals(func3,"010")){
		copy_str("lw",nome,2);
		return 2;
	}
	if(equals(func3,"100")){
		copy_str("lbu",nome,3);
		return 3;
	}
	if(equals(func3,"101")){
		copy_str("lhu",nome,3);
		return 3;
	}
	return 0;
}

int I_2_nome(char *func3,char *nome){

	
	if(equals(func3,"000")){
		copy_str("addi",nome,4);
		return 4;
	}
	if(equals(func3,"010")){
		copy_str("slti",nome,4);
		return 4;
	}
	if(equals(func3,"011")){
		copy_str("sltiu",nome,5);
		return 5;
	}
	if(equals(func3,"001")){
		copy_str("xori",nome,4);
		return 4;
	}
	if(equals(func3,"110")){
		copy_str("ori",nome,3);
		return 3;
	}
	if(equals(func3,"111")){
		copy_str("andi",nome,4);
		return 4;
	}
	if(equals(func3,"100")){
		copy_str("slli",nome,4);
		return 4;
	}
	return 0;

}

int I_4_nome(char *func3,char *nome){

	if(equals(func3,"001")){
		str_cpy("csrrw",nome);
		return 5;
	}
	if(equals(func3,"010")){
		str_cpy("csrrs",nome);
		return 5;
	}
	if(equals(func3,"011")){
		str_cpy("csrrc",nome);
		return 5;
	}
	if(equals(func3,"101")){
		str_cpy("csrrwi",nome);
		return 6;
	}
	if(equals(func3,"110")){
		str_cpy("csrri",nome);
		return 5;
	}
	if(equals(func3,"111")){
		str_cpy("csrrci",nome);
		return 6;
	}
	return 0;
}

int B_0_nome(char *func3,char *nome){

	if(equals(func3,"000")){
		str_cpy("beq",nome);
		return 3;
	}
	if(equals(func3,"100")){//por algum motivo na tabela e no objdump os func3 001 e 100 estao trocados...
		str_cpy("bne",nome);
		return 3;
	}
	if(equals(func3,"001")){
		str_cpy("blt",nome);
		return 3;
	}
	if(equals(func3,"101")){
		str_cpy("bge",nome);
		return 3;
	}
	if(equals(func3,"110")){
		str_cpy("bltu",nome);
		return 4;
	}
	if(equals(func3,"111")){
		str_cpy("bgeu",nome);
		return 4;
	}
	return 0;
}

int R_0_nome(char *func3,char *nome){

	if(equals(func3,"100")){
		str_cpy("sll",nome);
		return 3;
	}
	if(equals(func3,"010")){
		str_cpy("slt",nome);
		return 3;
	}
	if(equals(func3,"011")){
		str_cpy("sltu",nome);
		return 4;
	}
	if(equals(func3,"001")){
		str_cpy("xor",nome);
		return 3;
	}
	if(equals(func3,"110")){
		str_cpy("or",nome);
		return 2;
	}
	if(equals(func3,"111")){
		str_cpy("and",nome);
		return 3;
	}

}

int S_0_nome(char *func3,char *nome){

	if(equals(func3,"000")){
		copy_str("sd",nome,2);
		return 2;
	}
	if(equals(func3,"001")){
		copy_str("sh",nome,2);
		return 2;
	}
	if(equals(func3,"010")){
		copy_str("sw",nome,2);
		return 2;
	}
	return 0;

}
//... adicionar mais

void str_reverse(char *str,int strt,int end){


	while (strt <= end)
	{
		swap(&str[strt],&str[end]);
		strt++;
		end--;
	}
	

}

int get_nome(char *type,char *func3,char *nome){

	if(equals(type,"U_0")){
		return U_0_nome(func3,nome);
	}
	if(equals(type,"U_1")){
		return U_1_nome(func3,nome);
	}
	if(equals(type,"J_0")){
		return J_0_nome(func3,nome);
	}
	if(equals(type,"I_0")){
		return I_0_nome(func3,nome);
	}
	if(equals(type,"B_0")){
		return B_0_nome(func3,nome);
	}

	//adicionar mais...


	return 0;	

}

int get_bits(char *bits,int strt,int end,char *data){

	int c = 0;
	for(int i = strt;i <= end;i++){
		data[c] = bits[i];
		c++;
	}
	data[c] = '\0';
	return c;

}

int bin_dec(char *bin){

	//bin ta em little endian

	int len = str_len(bin);

	int curr = 0;
	int exp = 0;
	for(int i = 0;i < len;i++){

		int x = bin[i] - '0';
		curr += x*poww(2,exp);
		exp++;

	}
	return curr;
}

int int_to_string(int value,char *string){

	int c = 0;
	int negative = 0;
	if(value == 0){
		string[c] = '0';
		return 1;
	}
	if(value < 0){
		string[c] = '-';
		c++;
		value *= -1;
		negative = 1;
	}

	while(value > 0){
		string[c] = (value%10)+'0';
		value /= 10;
		c++;
	}

	string[c] = '\0';

	if(negative){
		str_reverse(string,1,c-1);
	}
	else{
		str_reverse(string,0,c-1);
	}
	return c;

}

//dado um valor binario em little edian e em complemento de 2,converter para decimal:

int xor(int a,int b){
	if(a == b){
		return 0;
	}
	return 1;
}

int bin_dec_little(char *bin){

	int len = str_len(bin);
	if(bin[len-1] == '0'){
		return bin_dec(bin);
	}
	else{

		//flipar os bits,somar 1 e converter para decimal

		//flipar:
		char bin_comp[50];
		for(int i = 0;i <= len;i++){
			if(i == len){
				bin_comp[i] = '\0';
				continue;
			}
			bin_comp[i] = !(bin[i]-'0') + '0';
		}
		//somar 1 no LSB:

		int overflow = 1;
		for(int i = 0;i < len;i++){

			int z = xor(overflow,bin_comp[i]-'0');
			
			if(overflow+(bin_comp[i]-'0') < 2){
				overflow = 0;
			}
			bin_comp[i] = z+'0';

		}
		return bin_dec(bin_comp)*-1;

	}
	return 0;

}

//shift left little endian : colocar n 0s a esquerda do LSB

int shift_left(char *bits,char *shifted,int n){

	int len = str_len(bits) + n;

	int c = 0;
	int i = 0;
	while (c < len)
	{
		if(c < n){
			shifted[c] = '0';
		}
		else{
			shifted[c] = bits[i];
			i++;
		}
		c++;
	}
	shifted[c] = '\0';
	return c;	
}

int shift_right(char *bits,char *shifted,int n){

	//pega o 1 valor e repete a direita do MSB

	int len = str_len(bits);
	int x = bits[len-1]-'0';
	len += n;

	int c = 0;
	int i = 0;
	while (c < len)
	{
		if(c < len-n){
			shifted[c] = bits[i];
			i++;
		}
		else{
			shifted[c] = x+'0';
		}
		c++;
	}
	shifted[c] = '\0';
	return c;

}

int imm_U(char *bits){

	//bits sao os bits que correspondem ao imm em little endian

	//adicionar 32-20 0s a esquerda do LSB
	char shifted[100];
	int len =  shift_left(bits,shifted,32-20);
	
	return bin_dec_little(shifted);
}


int imm_I(char *bits){

	//shift right 32-12 = 20
	char shifted[100];
	int len = shift_right(bits,shifted,20);
	return bin_dec_little(shifted);
}


void append(char* str1,char *str2,int num){

	//a ideia e dar um append de str2 no comeco de str1

	int c = 0;
	int k = str_len(str1);
	while (c < num || str1[k] != '\0' || str2[c] != '\0')
	{
		str1[k] = str2[c];
		c++;
		k++;
	}
	str1[k] = '\0';
	


}

int bin_little_endian(char *hex,char *bits){

	//dado um hex em little endian calcular seu valor em binario(little)

	//passar para big endian
	int len = str_len(hex);
	little_big_endian(hex,len/2);
	//printf("hex : %s\n",hex);

	//calcular o valor em binario invertendo as posicoes
	
	int c = 0;
	int strt;
	int end;
	for(int i = 0;i < len;i++){

		//1 digito hex --> 4 digitos bin

		int x = decod_unit(hex[i]);

		strt = c;
		end = c+3;
		for(int j = 0;j < 4;j++){

			bits[c] = (x%2)+'0';
			x /= 2;
			c++;

		}
		str_reverse(bits,strt,end);

	}
	bits[c] = '\0';

	strt = 0;
	end = c-1;

	str_reverse(bits,strt,end);
	
	

	return len*4;

}

void print_decod(char *decod,int len){

	for(int i = 0;i < len;i++){
		printf("%s",decod+50*i);
	}

}

int store_registor(char *bits,int str,int end,char *decod,int i){

	//retorna o ultimo index da array de strings utilizado

	char reg[50];
	get_bits(bits,str,end,reg);
	int reg_indx = bin_dec(reg);
	int len = str_len(registor_table[reg_indx]);
	str_cpy(registor_table[reg_indx],decod+50*i);
	i++;
	str_cpy(", ",decod+50*i);
	i++;
	return i;


}

int parcial_decod_I(char *bits,char *nome,char *decod){

	//se eu ja sei o nome da instrucao,o resto do processo e similar para todos os Is
	int i = 0;
	//nome --> baseado no func_type e no func3
	str_cpy(nome,decod+50*i);
	i++;
	str_cpy(" ",decod+50*i);
	i++;

	//rd:
	i = store_registor(bits,7,11,decod,i);

	//rs:
	i = store_registor(bits,15,19,decod,i);

	//imm:
	char imm_bin[50];
	get_bits(bits,20,31,imm_bin);
	int imm = imm_I(imm_bin);
	char imm_str[50];
	int_to_string(imm,imm_str);
	str_cpy(imm_str,decod+50*i);
	//printf("imm : %d\n",imm);
	i++;
	

	return i;

}

int decod_I_2(char *bits,char* decod){

	//pegar o func3
	char func3[10];
	get_bits(bits,12,14,func3);

	//slli tem func7 mas sempre e composoto somente de 0s
	if(!equals(func3,"101")){


	char nome[50];
	I_2_nome(func3,nome);

	return parcial_decod_I(bits,nome,decod);

	}
	

	return 0;

	//nome:
	

}

void format(char *tmp,char *decod,int i){

	//formatar de jalr ra, ra, 0 --> jalr ra, 0(ra)
	
	for(int j = 0;j <= 3;j++){
		str_cpy(tmp+50*j,decod+50*j);
	}

	char imm[10];
	char rs[10];

	str_cpy(tmp+50*4,rs);
	str_cpy(tmp+50*6,imm);

	int len1 = str_len(tmp+50*4);
	int len2 = str_len(tmp+50*6);

	char last[20];
	
	int c = 0;
	while (c < len2)
	{
		last[c] = imm[c];
		c++;
	}

	int k = 0;
	last[c] = '(';
	c++;
	while (k < len1)
	{
		last[c] = rs[k];
		c++;
		k++; 

	}
	last[c] = ')';
	last[c+1] = '\0';
	str_cpy(last,decod+50*4);
	

	

}

int imm_S(char *bits){

	//da um shift right em 20 posicoes no imm2 e da um append no imm1

	//imm1
	char imm1[100];
	get_bits(bits,7,11,imm1);
	

	//imm2
	char imm2[100];
	get_bits(bits,25,31,imm2);
	char imm2_shift[100];
	int len = shift_right(imm2,imm2_shift,20);
	
	append(imm1,imm2_shift,len);

	return bin_dec_little(imm1);


}

int decod_B_0(char *bits,char *decod,char *address,char *symb){

	//como eu tenho acesso a qual symb essa instrucao esta,esse e diferente

	//func3
	char func3[10];
	get_bits(bits,12,14,func3);

	//nome
	char nome[20];
	B_0_nome(func3,nome);

	int i = 0;
	str_cpy(nome,decod+50*i);
	i++;
	str_cpy(", ",decod+50*i);
	i++;

	//rs1
	i = store_registor(bits,15,19,decod,i);

	//rs2
	i = store_registor(bits,20,24,decod,i);

	//address
	char tmp[60] = "Ox";
	append(tmp,address,50);
	str_cpy(tmp,decod+50*i);
	i++;
	str_cpy(" ",decod+50*i);
	i++;

	//symb
	str_cpy(symb,decod+50*i);
	i++;

	return i;

}

int decod_S_0(char *bits,char *decod){

	char tmp[MAX];

	//como so tem 1 tipo S,essa funcao e geral
	int i = 0;
	//achar o func3
	char func3[10];
	get_bits(bits,12,14,func3);

	//nome:
	char nome[20];
	S_0_nome(func3,nome);

	str_cpy(nome,tmp+50*i);
	i++;
	str_cpy(" ",tmp+50*i);
	i++;

	//rs2:
	i = store_registor(bits,20,24,tmp,i);

	//rs1:
	i = store_registor(bits,15,19,tmp,i);

	//imm:
	int imm = imm_S(bits);
	char imm_str[50];
	int_to_string(imm,imm_str);
	str_cpy(imm_str,tmp+50*i);
	i++;

	format(tmp,decod,i);

	return i;

}

int decod_I_4(char *bits,char *decod){

	//achar o func3
	char nome[20];
	char func3[10];
	get_bits(bits,12,14,func3);

	//nome
	if(equals(func3,"000")){
		//func12
		char func12[20];
		get_bits(bits,20,31,func12);
		int value = bin_dec_little(func12);
		if(value != 0){
			str_cpy("ebreak",nome);
		}
		else{
			str_cpy("ecall",nome);
		}

		return parcial_decod_I(bits,nome,decod);//*(ver se realmente e isso)

	}
	else{
		I_4_nome(func3,nome);

		//2 tipos:

		if(equals(func3,"001") || equals(func3,"010") || equals(func3,"011")){
			return parcial_decod_I(bits,nome,decod);//*
		}
		else{

			int i = 0;
			str_cpy(nome,decod+50*i);
			i++;
			str_cpy(" ",decod+50*i);
			i++;

			//rd
			i = store_registor(bits,7,11,decod,i);

			//csr,valor padrao em binario
			char csr_bits[20];
			get_bits(bits,20,31,csr_bits);
			char csr_str[20];
			int_to_string(imm_I(csr_bits),csr_str);
			str_cpy(csr_str,decod+50*i);
			i++;
			str_cpy(", ",decod+50*i);
			i++;

			//zimm,o valor e tipico do imm_I
			char zimm_bits[20];
			get_bits(bits,15,19,zimm_bits);
			int zimm = imm_I(zimm_bits);
			char zimm_str[20];
			int_to_string(zimm,zimm_str);
			str_cpy(zimm_str,decod+50*i);
			i++;
			
			return i;

		}

	}

}

int decod_I_1(char *bits,char* decod){

	//achar o func3
	char func3[10];
	get_bits(bits,12,14,func3);
	//com o func3 e sabendo o tipo achar o nome(mnemonic)
	char nome[20];
	I_1_nome(func3,nome);
	char tmp[MAX];
	int x = parcial_decod_I(bits,nome,tmp);

	//print_decod(tmp,x);

	format(tmp,decod,x);
	return x-2;

	

}

int decod_I_0(char *bits,char *decod){
	
	char nome[50];
	
	//como para esse func_type(bits de 0-6) sa ha 1 possivel func3,n precisa procurar

	I_0_nome("",nome);
	char decod_tmp[MAX];
	int x = parcial_decod_I(bits,nome,decod_tmp);

	format(decod_tmp,decod,x);
	return x;
	


}

int decod_U_0(char *bits,char* decod){

	//dado 32 bits que eu sei que sao uma instrucao do tipo U_0 decodificar:

	//como nesse caso so ha 1 instrucao o func3 nao importa

	U_0_nome("",decod);//decod[0]
	str_cpy(" ",decod+50);//decod[1]
	//pegando o rd:
	char rd_bin[50];
	get_bits(bits,7,11,rd_bin);
	int rd_idx = bin_dec(rd_bin);
	int len = str_len(registor_table[rd_idx]);
	copy_str(registor_table[rd_idx],decod+50*2,len);//decod[2]
	str_cpy(", ",decod+50*3);

	//pegando o imm
	char imm_bin[50];
	get_bits(bits,12,31,imm_bin);
	int imm_int = imm_U(bits);
	char imm_str[50];
	len = int_to_string(imm_int,imm_str);
	str_cpy(imm_str,decod+50*4);

	return 5;

}

int decod_U_1(char *bits,char* decod){

	//dado 32 bits que eu sei que sao uma instrucao do tipo U_0 decodificar:

	//como nesse caso so ha 1 instrucao o func3 nao importa

	U_1_nome("",decod);//decod[0]
	str_cpy(" ",decod+50);//decod[1]
	//pegando o rd:
	char rd_bin[50];
	get_bits(bits,7,11,rd_bin);
	int rd_idx = bin_dec(rd_bin);
	int len = str_len(registor_table[rd_idx]);
	copy_str(registor_table[rd_idx],decod+50*2,len);//decod[2]
	str_cpy(", ",decod+50*3);

	//pegando o imm
	char imm_bin[50];
	get_bits(bits,12,31,imm_bin);
	int imm_int = bin_dec(imm_bin);
	char imm_str[50];
	len = int_to_string(imm_int,imm_str);
	str_cpy(imm_str,decod+50*4);

	return 5;

}

int decod_R_0(char *bits,char *decod){

	//func3
	char func3[10];
	get_bits(bits,12,14,func3);

	char nome[20];

	if(equals(func3,"000")){

		//func7
		char func7[20];
		get_bits(bits,25,31,func7);
		int value = bin_dec_little(func7);
		if(value != 0){
			str_cpy("sub ",nome);
		}
		else{
			str_cpy("add ",nome);
		}

	}
	if(equals(func3,"101")){
		//func7
		char func7[20];
		get_bits(bits,25,31,func7);
		int value = bin_dec_little(func7);
		if(value != 0){
			str_cpy("sra ",nome);
		}
		else{
			str_cpy("or ",nome);
		}
	}
	else{
		R_0_nome(func3,nome);
	}

	int i = 0;
	str_cpy(nome,decod+50*i);
	i++;
	str_cpy(" ",decod+50*i);

	//rd
	i = store_registor(bits,7,11,decod,i);

	//rs1
	i = store_registor(bits,15,19,decod,i);

	//rs2
	i = store_registor(bits,20,24,decod,i);
	i--;

	return i;

}


int decod_instruction(char *type,char *bits,char* decod){

	if(equals(type,"U_0")){
		return decod_U_0(bits,decod);
	}
	if(equals(type,"U_1")){
		return decod_U_1(bits,decod);
	}
	if(equals(type,"I_0")){
		return decod_I_0(bits,decod);
	}
	if(equals(type,"I_1")){
		return decod_I_1(bits,decod);
	}
	if(equals(type,"I_2")){
		return decod_I_2(bits,decod);
	}
	if(equals(type,"S_0")){
		return decod_S_0(bits,decod);
	}
	if(equals(type,"I_4")){
		return decod_I_4(bits,decod);
	}
	if(equals(type,"R_0")){
		return decod_R_0(bits,decod);
	}
	return 0;

}
