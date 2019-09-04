#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


int getLineCount(char* fn) {
	FILE *fileptr;
    int count_lines = 0;
    fileptr = fopen(fn, "r");
    char chr = getc(fileptr);
    while (chr != EOF)
    {
        //Count whenever new line is encountered
        if (chr == '\n')
        {
            count_lines = count_lines + 1;
        }
        //take next character from file.
        chr = getc(fileptr);
    }
    fclose(fileptr); //close file.
    printf("There are %d lines in %s\n", count_lines, fn);
    return count_lines;
}

void loadVocabulary(char* file1, char* file2, char** nouns, char** adjectives) {
	printf("loading %s and %s\n", file1, file2);
	int i = 0;
	FILE *fp;
	fp = fopen(file1, "r");
	char buff[32];
	while(fscanf(fp, "%32s", buff) != EOF) {
		nouns[i] = (char *) malloc(sizeof(buff));
		memcpy(nouns[i], buff, sizeof(buff));
		//printf("noun: %s\n", &nouns[i]);
		i++;
	}
	fclose(fp);
	fp = fopen(file2, "r");
	i = 0;
	while(fscanf(fp, "%32s", buff) != EOF) {
		adjectives[i] = (char *) malloc(sizeof(buff));
		memcpy(adjectives[i], buff, sizeof(buff));
		//printf("adjectives: %s\n", adjectives[i]);
		i++;
	}
	fclose(fp);
	printf("loaded vocabulary\n");
}

struct Location
{
	char name[64];
	int coordinates[2];
};

struct Cargo
{
	char name[64];
	float size;
	int pickup_location;
	int dropoff_location;
	int pickup_deadline;
	int dropoff_deadline;
};

struct Ship
{
	int location[2];
	int destination;
	float time_to_destination;
	int cargo[256];
	int cargo_amount;
	int cargo_capacity;
	float speed;
	float operating_cost;
	float fuel_range;
};

struct Contract
{
	int cargo[16];
	int cargo_count;
	double reward;
	double top_bid;
	int top_bidder;
};

struct Scheduler
{
	char name[64];
	double cash;
	int ships[256];
	int ship_count;
	int outstanding_bids[256];
	int bid_count;
};


void printScheduler(struct Scheduler t) {
	printf("Scheduler %s:\n", t.name);
	printf("cash: %f, ship_count: %d, bid_count: %d \n", t.cash, t.ship_count, t.bid_count);
	int i = 0;
	while(i < t.ship_count) {
		printf("ship %d: %d\n", i, t.ships[i]);
		i++;
	}
	i = 0;
	while(i < t.bid_count) {
		printf("bid %d: %d\n", i, t.outstanding_bids[i]);
		i++;
	}
}

void printContract(struct Contract t, int i) {
	printf("Contract %d is for $%.2f and involves %d cargos\n", i, t.top_bid, t.cargo_count);
	printf("starting bid: $%.2f, top bidder: %d\n", t.reward, t.top_bidder);
}

int generateContracts(char* cfile, struct Contract* contracts, int contract_count, struct Cargo* cargos, char** nouns, 
					char** adjectives, int noun_count, int adjective_count, struct Location* locations, int location_count) {
	printf("Generating %d Contracts...\n", contract_count);
	FILE *fp;
	fp = fopen(cfile, "w+");
	int cargo_count = 0;
	int i = 0;
	while(i < contract_count) {
		struct Contract t;
		int c_amt = (rand() % 16) + 1;
		fprintf(fp, "Contract %d has %d items:\n", i, c_amt);
		int z = 0;
		while(z < c_amt) {
			struct Cargo c;
			c.size = ((rand() % 100) + 1)*0.1;
			c.pickup_location = rand() % location_count;
			c.dropoff_location = rand() % location_count;
			while(c.dropoff_location == c.pickup_location) {
				c.dropoff_location = rand() % location_count;
			}
			c.pickup_deadline = rand() % 1000;
			c.dropoff_deadline = c.pickup_deadline + (rand() % 1000);
			int adj_r = rand() % adjective_count;
			int noun_r = rand() % noun_count;
			//printf("%d\n", c.pickup_deadline);
			strcpy(c.name, adjectives[adj_r]);
			strcat(c.name, " ");
			strcat(c.name, nouns[noun_r]);
			memcpy(&cargos[cargo_count], &c, sizeof(struct Cargo));
			fprintf(fp, "    '%s' (size: %.2f) shipping from %s by %d hours to %s by %d hours\n",
				 	cargos[cargo_count].name, c.size, locations[cargos[cargo_count].pickup_location].name, cargos[cargo_count].pickup_deadline,
				  	locations[cargos[cargo_count].dropoff_location].name, cargos[cargo_count].dropoff_deadline);
			cargo_count++;
			z++;
		}
		t.reward = c_amt * ((rand() % 10000) + 100.0);
		t.cargo_count = c_amt;
		t.top_bid = t.reward;
		t.top_bidder = -1;
		fprintf(fp, "    reward: %0.2f\n", t.reward);
		memcpy(&contracts[i], &t, sizeof(struct Contract));
		i++;
	}
	fclose(fp);
	printf("wrote contract details to %s\n", cfile);
	return cargo_count;
}

double evaluateFitness(int simulation_length, struct Scheduler adam, struct Location locations,
			 struct Ship ships, struct Cargo cargos, char* genome, int genome_size) {
	int wait = 0;
	int genome_step = 0;
	int hour = 0;
	while(hour < simulation_length) {
		if(wait == 0 && genome_step < genome_size) {
			//process genome
			genome_step++;
		} else {
			//run an hour of simulation
			wait--;
		}

		hour++;
	}
	return 0.0;
}


int main(int argc, const char* argv[])
{
	printf("\nScheduler Program\n\n");
	printf("sizeof char: %ld\n", sizeof(char));
	srand(time(0));
	int i;

	struct Location locations[3];
	int location_count = 3;
	strcpy(locations[0].name, "Port Isoclous");
	strcpy(locations[1].name, "Port Chappero");
	strcpy(locations[2].name, "Port Melody");
	locations[0].coordinates[0] = 0.0;
	locations[0].coordinates[1] = 0.0;
	locations[1].coordinates[0] = 2.0;
	locations[1].coordinates[1] = 0.0;
	locations[2].coordinates[0] = 0.0;
	locations[2].coordinates[1] = 5.0;

	char file1[] = "nouns.txt";
	char file2[] = "adjectives.txt";
	char file3[] = "contracts.txt";

	int noun_count = getLineCount(file1);
	int adjective_count = getLineCount(file2);

	char** nouns = malloc(noun_count * sizeof(char *));
	char** adjectives = malloc(adjective_count * sizeof(char *));

	loadVocabulary(file1, file2, nouns, adjectives);

	int contract_count = 12;
	struct Cargo* cargos = (struct Cargo*) malloc(contract_count*16*sizeof(struct Cargo));
	struct Contract* contracts = (struct Contract*) malloc(contract_count*sizeof(struct Contract));
	int cargo_count = generateContracts(file3, contracts, contract_count, cargos, nouns, adjectives, noun_count, adjective_count, locations, location_count);
	printf("%d cargo generated\n", cargo_count);

	int population_count = 10;
	int generation_count = 10;
	int genome_size = 50*32;

	unsigned char** genomes = malloc(population_count * sizeof(char *));
	i = 0; 
	while(i < population_count) {
		genomes[i] = (char *) malloc(genome_size*sizeof(char));
		int x = 0;
		while(x < genome_size) {
			genomes[i][x] = (char) random();
			x++;
		}
		i++;
	}

	int simulation_length = 2000;
	double *fitnesses = (double *)malloc(sizeof(double)*population_count);

	printf("beginning simulation...\n");

	int generation = 0;
	while(generation < generation_count) {
		//STEP 1: evaluate the fitness of the population
		printf("generation %d\n", generation);
		int current_target = 0;
		while(current_target < population_count) {
			printf("evaluating subject %d\n", current_target);
			int wait = 0;
			int genome_step = 0;
			int hour = 0;

			struct Scheduler adam = {.cash=1000.0, .ships[0]=0, .ship_count=1, .bid_count=0};
			strcpy(adam.name, "Adam");

			struct Ship ships[1];
			int ship_count = 1;
			ships[0].location[0] = 0.0;
			ships[0].location[1] = 0.0;
			ships[0].destination = 0;
			ships[0].time_to_destination = 0;
			ships[0].cargo_amount = 0;
			ships[0].cargo_capacity = 1000;
			ships[0].speed = 10.0;
			ships[0].operating_cost = 0.0;
			ships[0].fuel_range = 100.0;

			while(hour < simulation_length) {
				//printf("hour %d\n", hour);
				if(wait == 0 && genome_step < genome_size) {
					//process genome
					switch((unsigned int) (genomes[current_target][genome_step] >> 6)) {
						case 0: //wait
							wait += (unsigned int) genomes[current_target][genome_step + 1];
							printf("wait %d\n", wait);
							break;
						case 1: //pickup cargo
							printf("pickup cargo\n");
							break;
						case 2: //dropoff cargo
							printf("dropoff cargo\n");
							break;
						case 3: //direct ship
							printf("direct ship\n");
							break;
						default:
							//how did it get here?
							printf("genome type switch reached default?!");
					} 
					genome_step += 4;
				} else {
					//run an hour of simulation
					wait--;
				}

				hour++;
			}
			current_target++;
		}
		//STEP 2: construct the roulette

		generation++;
	}

	i = 0; while(i < population_count) {free(genomes[i]); i++;}
	i = 0; while(i < noun_count) {free(nouns[i]); i++;}
	i = 0; while(i < adjective_count) {free(adjectives[i]); i++;}
	free(contracts);
	free(cargos);
	//free(&adjectives);
	//free(&nouns);
}
