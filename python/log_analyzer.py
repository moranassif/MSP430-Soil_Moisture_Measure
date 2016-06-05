import matplotlib.pyplot as plt

class LogAnalyzer(object):
    START_LINE = 513
    BYTES_PER_LINE = 16
    INTS_PER_ENTRY = 4
    NIBLES_PER_INT = 4

    def __init__(self, file_path):
        f = open(file_path)
        self._data = "".join(f.readlines()[self.START_LINE:])
        self._data = self._data[:self._data.find("FF")].replace("\n", "").replace(" ", "")

        print(self._data)
        self._time = []
        self._values = []
        self._is_button = []
        day = 0
        previous_hour = -1
        for i in range(0, len(self._data), self.INTS_PER_ENTRY * self.NIBLES_PER_INT):
            hour = self._read_int_on_index(self._data, i + 0 * self.NIBLES_PER_INT)
            minutes = self._read_int_on_index(self._data, i + 1 * self.NIBLES_PER_INT)
            value = self._read_int_on_index(self._data, i + 2 * self.NIBLES_PER_INT)
            button = self._read_int_on_index(self._data, i + 3 * self.NIBLES_PER_INT) == 0x17
            # Adjust days
            if previous_hour > 12 > hour:
                day += 1
            previous_hour = hour
            self._time.append(day * 24 + hour * 60 + minutes)
            self._values.append(value)
            self._is_button.append(button)
        plt.plot(self._time, self._values)
        plt.show()


    @staticmethod
    def _read_int_on_index(data, index):
        # Endianity
        intstr = data[index+2:index+4] + data[index:index+2]
        return int(intstr, 16)



if __name__ == "__main__":
    log_analyzer = LogAnalyzer(r"C:\TI\MSPFlasher_1.3.8\flash_new.txt")
