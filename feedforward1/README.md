Feedforward Neural Network trained by backpropagation.

!!! The params of the network need to be set to appropriate values:

- number of hidden layers;
- number of neurons in each hidden layer;
- epochs = number of trainings over the data set;


Other values and functions to be specified:
- the activation funcion (sigmoid, tanh)
- weights ranges
- the learning rate - in updating the weights 
- criteria for correct classification

Improving network performance:
- re-scaling the input (for example: in [-1, 1])
- decreasing the learning rate during training
- randomizing the order of samples in the training set


This example trains the network over the "all.txt" data set.
It learns to correctly classify the symbols 0-9, a-z, A-Z.