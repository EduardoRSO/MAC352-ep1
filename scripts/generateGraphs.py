import matplotlib.pyplot as plt
import os
import numpy as np

#categories = ['0','10','100']

categories = ['0', '10', '100']

def create_CPU_dispersion_chart(cpu_values, number_of_consumers):
    plt.scatter(list(range(1,101)), cpu_values)
    plt.xlabel('Seconds')
    plt.ylabel('CPU usage(%)')
    plt.title('Number of consumers ='+str(number_of_consumers))
    plt.yticks(np.arange(0,20,step=5))
    plt.show()
    #plt.savefig('CPU_'+str(number_of_consumers)+'.png')
    

def create_NET_I_dispersion_chart(net_i_values, number_of_consumers):
    plt.scatter(list(range(1,101)), net_i_values)
    plt.xlabel('Seconds')
    plt.ylabel('Network input(kB)')
    plt.title('Number of consumers ='+str(number_of_consumers))
    plt.yticks(np.arange(0,5000,step=1000))
    plt.show()
    #plt.savefig('NET_I_'+str(number_of_consumers)+'.png')
    
def create_NET_O_dispersion_chart(net_o_values, number_of_consumers):
    plt.scatter(list(range(1,101)), net_o_values)
    plt.xlabel('Seconds')
    plt.ylabel('Network output(kB)')
    plt.title('Number of consumers ='+str(number_of_consumers))
    plt.yticks(np.arange(0,5000,step=1000))
    plt.show()
    #plt.savefig('NET_O_'+str(number_of_consumers)+'.png')

def mean(list):
    return sum(list) / len(list)

def create_CPU_bar_chart(a,b,c):
    values = [mean(a['CPU']), mean(b['CPU']), mean(c['CPU'])]
    plt.bar(categories, values)
    plt.xlabel('Number of consumers')
    plt.ylabel('CPU(%) usage')
    plt.show()

def create_NET_I_bar_chart(a,b,c):
    values = [sum(a['NET_I']), sum(b['NET_I']), sum(c['NET_I'])]
    plt.bar(categories, values)
    plt.xlabel('Number of consumers')
    plt.ylabel('Network input(kB)')
    plt.show()    

def create_NET_O_bar_chart(a,b,c):
    values = [sum(a['NET_O']), sum(b['NET_O']), sum(c['NET_O'])]
    plt.bar(categories, values)
    plt.xlabel('Number of consumers')
    plt.ylabel('Network output(kB)')
    plt.show()  

def convert_to_kb(entry):
    if(len(entry)<3):
        entry = float(entry[:-1])/1000
    elif entry[-2:] == 'MB':
       entry = entry[:-2]
       entry = float(entry)*1024
    elif entry[-2:] == 'kB':
       entry = float(entry[:-2]) 
    return entry

def get_data(number_of_consumers):
    d = {'CPU': [], 'NET_I': [], 'NET_O': []}
    with open('/home/erso/Documents/MAC352/MAC352-ep1/output/'+str(number_of_consumers)+'.txt','r') as file:
        for line in file:
            d['CPU'].append(float(line.split()[0].replace('%','')))
            d['NET_I'].append(convert_to_kb(line.split()[2]))
            d['NET_O'].append(convert_to_kb(line.split()[4]))
    return d



def main():
    #for number_of_consumers in categories:
        a = get_data(0)
        b = get_data(10)
        c = get_data(100)
        create_CPU_bar_chart(a,b,c)
        create_NET_I_bar_chart(a,b,c)
        create_NET_O_bar_chart(a,b,c)
        #create_CPU_dispersion_chart(d['CPU'],number_of_consumers)
        #create_NET_I_dispersion_chart(d['NET_I'],number_of_consumers)
        #create_NET_O_dispersion_chart(d['NET_O'],number_of_consumers)
main()
