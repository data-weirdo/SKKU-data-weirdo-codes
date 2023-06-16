#define _CRT_SECURE_NO_WARNINGS

#define max_process_per_queue 100 // [����] �� ť���� �ִ�� ��� �� �ִ� process�� ��: 100��
#define max_process 100 // [����] �Է����� ���� �� �ִ� process�� ��: 100��
#define max_computing_time 10000
#define total_queue 3 // ��ü ť�� ���� (������ ����) 
#define q0 0 // Q0
#define q1 1 // Q1
#define q2 2 // Q2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// �� process ������ ������ �����ϴ� ����ü
typedef struct process_info {
	int process_num; // �Է¹޴� �� (process�� ��ȣ)
	int arrival_time, burst_time; // �Է¹޴� �� 
	int turnaround_time, waiting_time, start_time, end_time; // ����� �� ��
	int run_time; 
} process;

// �� process�� ������ ����ü���� �迭
process pc_address[max_process]; 

int process_in_cpu[max_computing_time]; // cpu ������ ���� ���� process�� ��ȣ�� ������ �迭 

// �� queue�� process���� ���� ��
typedef struct queue {
	process *queue[max_process_per_queue]; // ����ü �� ������ ����.
	int first, last, length;
} queue;

// ť �ʱ�ȭ
void init_queue(queue *q){ 
	int i;

	for (i=0; i<max_process_per_queue; i++){
		(*q).queue[i] = NULL;
	}
	
	(*q).first = 0;
	(*q).last = 0;
	(*q).length = 0;
}

// �� ť���� Ȯ��
int queue_isempty(queue *q){
	if(q -> length == 0) return 1; // Queue: Empty
	else return 0; // Queue: Not Empty 
}

// ť�� Push
void push_to_queue(queue *q, process *p){

	int i, j, min, index;
	process *temp = NULL;

	(*q).queue[q -> last] = p;
	(*q).last += 1;
	(*q).length += 1;

	// push�� ���Ŀ��� �̸� arrival time ������ queue ���� ��� process ����
	// �������� �˰��� ��� 
	for(i=(*q).first; i<(*q).last;i++){
		min=9999;
		for(j=i; j<(*q).last; j++){
			if(min > q->queue[j]->arrival_time){
				min = q->queue[j]->arrival_time;
				index = j;
			}
		}
		temp = (*q).queue[i];
		(*q).queue[i] = (*q).queue[index];
		(*q).queue[index] = temp;
	}
}

// ť�κ��� Pop
process *pop_from_queue(queue *q, int cpu_time){

	process *pop = NULL;

	if(queue_isempty(q)==0){
		if((*q).queue[(*q).first]->arrival_time <= cpu_time){
			pop = (*q).queue[(*q).first];
			(*q).first++;
			(*q).length--;
		}
	}
	return pop;
}

int in_progress(process *p, int cpu_time){

	(*p).run_time++;

	// ó�� ���Դٸ� ����� cpu_time�� start_time�� �Ҵ�. 
	if((*p).start_time == -999){
		(*p).start_time = cpu_time;
	}

	if((*p).burst_time == (*p).run_time){
		(*p).end_time = cpu_time;
		return 1;
	}
	else return 0;	
}

int mfq(int cnt, int cpu_time){

	queue ready_queue[total_queue];
	process *present_pcs=NULL;
	process *next_pcs=&pc_address[0];
	int total_execution_time=0;

	int num_of_finish = 0; // �� ���μ����� �Ϻ��� ����Ǹ� +1
	int message=0; // q0���� arrival_time�� cpu_time�� ��ġ�� ���� 1, ������ 0
	int position=0; // �ش� cpu_time���� ���� �켱������ �ִ� ť ��ȣ (0,1,2)

	// ť ����
	init_queue(&ready_queue[q0]);
	init_queue(&ready_queue[q1]);
	init_queue(&ready_queue[q2]);

	// ��Ʈ ��Ʈ �׷��ִ� ��, �ϴ� ���� �� ������ ���߿� ��������
	for(cpu_time; num_of_finish < cnt; cpu_time++){
		
		total_execution_time++;
		// arrival_time�� cpu_time�� �������� �ڵ����� q0���� ���ֵ���. 
		message = 0;
		if((*next_pcs).arrival_time == cpu_time){
			message = 1;
			push_to_queue(&ready_queue[q0], next_pcs);
			next_pcs++;
		}

		// ���� ���� ���μ����� ���� ���
		if(present_pcs == NULL){
			position = 0;
			if(queue_isempty(&ready_queue[q0]) == 0){
				present_pcs = pop_from_queue(&ready_queue[q0], cpu_time);
				process_in_cpu[cpu_time] = present_pcs->process_num; // �߰�
				position = q0;
			}

			else if (queue_isempty(&ready_queue[q0]) == 1 && queue_isempty(&ready_queue[q1]) == 0){
				present_pcs = pop_from_queue(&ready_queue[q1], cpu_time);
				process_in_cpu[cpu_time] = present_pcs->process_num; // �߰�
				position = q1;
			}

			else{
				present_pcs = pop_from_queue(&ready_queue[q2], cpu_time);
				process_in_cpu[cpu_time] = present_pcs->process_num; // �߰�
				position = q2;
			}
			
			cpu_time--;
			total_execution_time--;
		}
		
		else{
			// �� ��° Ȥ�� �� ��° ť���� ���� ���� ��, �ٸ� ���μ����� '��' �����ϴ� ���
			if(position != q0 && message == 1){ 
				push_to_queue(&ready_queue[position], present_pcs);
				present_pcs = pop_from_queue(&ready_queue[q0], cpu_time);
				process_in_cpu[cpu_time] = present_pcs->process_num; // �߰�
				position = q0;
			}

			if(in_progress(present_pcs, cpu_time) == 0){
				
				if((*present_pcs).run_time == 2){
					push_to_queue(&ready_queue[q1], present_pcs);
					process_in_cpu[cpu_time] = present_pcs->process_num;
					present_pcs = NULL;
				}

				else if((*present_pcs).run_time == 6){
					push_to_queue(&ready_queue[q2], present_pcs);
					process_in_cpu[cpu_time] = present_pcs->process_num;
					present_pcs = NULL;
				}

				else process_in_cpu[cpu_time] = present_pcs->process_num;
			}

			else{
				num_of_finish ++;
				process_in_cpu[cpu_time] = present_pcs->process_num;
				present_pcs = NULL;
			}
		}
	}
	return total_execution_time;
}


void result_table(int cnt){ // cnt: process cnt
	
	int i;
	int tt_sum=0, wt_sum=0;

	puts("+-----+------------+------------------+-----------------+");
	puts("| PID | Burst Time | Turnaround Time  |   Waiting Time  |");
	puts("+-----+------------+------------------+-----------------+");

	for(i=0; i<cnt; i++){
		printf("| %2d  |     %2d     |        %2d        |       %2d        |\n", 
			pc_address[i].process_num, pc_address[i].burst_time, pc_address[i].turnaround_time, pc_address[i].waiting_time);
	}
	printf("+-----+------------+------------------+-----------------+\n");
	printf("\n");

	for(i=0; i<cnt; i++){
		tt_sum += pc_address[i].turnaround_time;
		wt_sum += pc_address[i].waiting_time;
	}

	printf("��ü ���μ����� ��� TT: %d\n", tt_sum/cnt);
	printf("��ü ���μ����� ��� WT: %d\n", wt_sum/cnt);
	printf("\n");
}


void for_gantt_chart(int total){ 

	// ���δ� �ݺ����� ������ ���� �ӽú����� ��. 
	int i, j;
	int p=0, q, cnt;
	int hyphen_cnt=0, gantt_temp; // hyphen_cnt: ��Ʈ ��Ʈ�� �� ���� ���� gantt_temp: ������ ����Ǿ� ������ ���μ����� �� ���� ������ ���μ����� ���ڱ����� ����
	int temp1, temp2, temp3=0, length=0;

	puts(	"Gantt Chart"	);
	printf("+");
	for(i=0;i<total;i++){
		if(i<(total-1)) {
			printf("---");
			hyphen_cnt+=3;
		}
		if(i==(total-1)) {
			printf("--+\n");
			hyphen_cnt+=2;
		}
	}
	printf("|");
	
	while(p<total){
		cnt=1;
		for(q=p+1;q<total; q++){
			if(process_in_cpu[p]==process_in_cpu[q]) cnt++;
			else break;
		}
		for(temp1=0; temp1<cnt; temp1++){
			printf(" ");
		}
		if(cnt>4) printf(" ");

		printf("%d", process_in_cpu[p]);
		for(temp1=0; temp1<cnt; temp1++){
			printf(" ");
		}
		if(cnt>4) printf(" ");
		printf("|");
		p += cnt;
	}

	printf("\n");
	printf("+");
	for(i=0;i<total;i++){
		if(i<(total-1)) printf("---");
		if(i==(total-1)) printf("--+\n");
	}

	printf("   ");

	temp2 = 0;
	i = temp2;
	do{
		for(j=i+1; j<total; j++){
			if (process_in_cpu[i] == process_in_cpu[j]){
				temp2++;
			}
			else {
				length = temp2-i+1;
				temp2++;
				break;
			}
		}

		if(length>1){
			for(temp3=0; temp3<length; temp3++) {
				printf(" ");
				if (temp3 >= 3) printf(" ");
			}

		}
		else printf(" ");

		// ť�� �̵��� �ڿ���, ������ ���μ����� ���޾� CPU�� ��� �����ϰ� �� ���� Gantt Chart ǥ�⸦ ���� ���� ó�� 

		if(temp2-i<=4 && process_in_cpu[temp2] != process_in_cpu[i]) printf("%2d", temp2);
		
		if(length>1){
			for(temp3=0; temp3<length; temp3++) printf(" ");
		}
		else printf(" ");

		i = temp2;
		gantt_temp = i;

		if (i==(total-1)) printf("%d\n", total);
		

	}while(i!=(total-1));
	
	printf("\n");
}

int main(void){

	int process_cnt; // process_cnt: �Է½� ���� �� process�� ����
	int i=-1;
	int k, l, temp;
	int fore, back, sign;
	int cpu_time = 0; // 1�� ������ �� iteration���� ����	
	int total = 0;

	FILE *fp = fopen("/My Directory","rt");

	if(fp==NULL){
		printf("���Ͽ��� ����\n");
	}
	
	else{
		while(!feof(fp))
		{
			if(i==-1){
				fscanf(fp, "%d", &process_cnt);
			}
			else {
				fscanf(fp, "%d %d %d", &pc_address[i].process_num, &pc_address[i].arrival_time, &pc_address[i].burst_time);
				pc_address[i].turnaround_time = 0;
				pc_address[i].waiting_time = 0;
				pc_address[i].start_time = -999; // ó�� ���� process�� ���� ����x
				pc_address[i].end_time = 0;
				pc_address[i].run_time = 0;
			}
			++i;
		}
	}

	total += mfq(process_cnt, cpu_time);
	
	for(k=0; k<process_cnt; k++){
		temp = 0; 
		fore = 0; 
		back = 0; 
		sign = 0;
		for (l=0; l<total; l++){
			if(process_in_cpu[l] == (k+1)){
				if(sign==0){
					fore=temp;
					sign++;
				}
				else{
					back=temp+1;
				}
			}
			temp++;
		}
		pc_address[k].start_time = fore;
		pc_address[k].turnaround_time = back-pc_address[k].arrival_time;
		pc_address[k].waiting_time = pc_address[k].turnaround_time - pc_address[k].burst_time;
	}

	
	result_table(process_cnt);
	for_gantt_chart(total);

	system("pause");
	
	return 0;
}