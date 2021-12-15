#!/usr/bin/env python3
import torch
# import numpy as np
import sys

NOTTAKEN = 0
TAKEN = 1

class NN(torch.nn.Module):
    def __init__(self):
        super(NN, self).__init__()
        self.fc1 = torch.nn.Linear(3, 2)

    def forward(self, input):
        x = self.fc1(input)
        #x = torch.relu(x)
        #x = self.fc2(x)
        return x


myNet = NN()

def read():
    raw = []
    for i,line in enumerate(reversed(open("his.txt").readlines())):
        raw.append([int(x) for x in line.split(' ')])
        if i == 10000:
            break
    data = torch.tensor(raw)
    # data = np.loadtxt('his.txt')
    pc = torch.tensor(data[:, :-1]).float()
    outcome = torch.tensor(data[:, -1]).long()
    dataset = torch.utils.data.TensorDataset(pc, outcome)
    return torch.utils.data.DataLoader(dataset, shuffle=True)

def train_predictor():
    history = read()
    net = NN()
    best = float('inf')
    optimizer = torch.optim.SGD(net.parameters(), lr=0.01)
    lossFunc = torch.nn.CrossEntropyLoss()
    for i in range(50):
        for (pc, outcome) in history:
            optimizer.zero_grad()
            output = net(pc)
            loss = lossFunc(output, outcome)
            loss.backward()
            optimizer.step()
        with torch.no_grad():
            sum = 0
            for (pc, outcome) in history:
                output = net(pc)
                loss = lossFunc(output, outcome)
                sum += loss
            ave = sum / len(history)
            if ave < best:
                best = ave
                torch.save(net.state_dict(), './mynn.pt')

    # myNet.load_state_dict(torch.load('./mynn.pt'))
    

def make_prediction(pc, ghistory, lhistory):
    myNet.load_state_dict(torch.load('./mynn.pt'))
    out = myNet(torch.tensor([[pc, ghistory, lhistory]]).float())
    predictions = torch.argmax(out.data, 1)
    return predictions.item()

if __name__ == '__main__':
    if sys.argv[1] == 'train':
        train_predictor()
    else:
        # make_prediction(int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
        print(make_prediction(int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4])))
