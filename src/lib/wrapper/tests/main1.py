class MainClass:
    def __init__(self):
        pass

    def a_function(self):
        a = []
        for i in range(100):
            a.append(i)
        print(a)

def main():
    obj = MainClass()
    obj.a_function()

main()
