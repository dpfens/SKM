import unittest
import markov
import skm


class MarkovChain(unittest.TestCase):

    def setUp(self):
        import random
        self.rows = 500
        self.columns = 500
        states = 5
        self.states = list(range(states + 1))
        sequences = [[random.randint(0, states) for j in range(self.columns)] for i in range(self.rows)]
        self.sequences = sequences
        sequences = [[random.randint(0, states) for j in range(self.columns)] for i in range(self.rows)]
        self.additional_sequences = sequences

    def test_build(self):
        matrix = markov.MarkovMatrix(self.states)
        matrix.build(self.sequences)

    def test_update(self):
        matrix = markov.MarkovMatrix(self.states)
        matrix.build(self.sequences)
        matrix.update(self.additional_sequences)

    def test_predict(self):
        matrix = markov.MarkovMatrix(self.states)
        matrix.build(self.sequences)
        print(repr(matrix))
        predictions = matrix.predict(None, 3)
        print(predictions)


class SKM(unittest.TestCase):

    def setUp(self):
        import random
        self.rows = 5000
        self.columns = 500
        states = 7
        self.states = range(states + 1)
        sequences = [[random.randint(0, states) for j in range(self.columns)] for i in range(self.rows)]
        self.sequences = sequences
        self.k = 5

    def test_build(self):
        pass
        #clusters = skm.skm(self.sequences, self.k, 1000, 1)


if __name__ == '__main__':
    unittest.main()
