#include <iostream>

class SomeThing {
private:
    int a_ = 0;
    char *chs_ = nullptr;

public:
    SomeThing() {
        std::cout << "[Special method invoked] ===> default constructor invoked, " << "new obj address: "
                  << (void *) this << std::endl;
        report();
    }

    // modified with explicit if you want to avoid unwanted type conversion
    SomeThing(int a) {
        this->a_ = a;
        std::cout << "[Special method invoked] ===> normal constructor1 invoked, " << "new obj address: "
                  << (void *) this << std::endl;
        report();
    }


    // modified with explicit if you want to avoid unwanted type conversion
    SomeThing(const char *str, unsigned long len) {
        std::cout << "[Special method invoked] ===> normal constructor2 invoked, " << "new obj address: "
                  << (void *) this << std::endl;
        this->chs_ = new char[len + 1];
        strncpy(chs_, str, len);
    }

    SomeThing(const SomeThing &other_thing) {
        std::cout << "[Special method invoked] ===> copy___ constructor invoked, "
                  << "new obj address: "
                  << (void *) this << " copied__ obj address: "
                  << (void *) &other_thing << std::endl;
        this->a_ = other_thing.a_;
        this->DealWithChs(this->chs_, other_thing.chs_);
        report();
    }

private:
    void DealWithChs(char *&this_chs, char *const &other_chs) {
        if (other_chs == nullptr) {
            delete[] this_chs;
            this_chs = nullptr;
        } else {
            unsigned long len = strlen(other_chs);
            this_chs = new char[len + 1];
            strcpy(this_chs, other_chs);
        }
    }

public:

    // note that the arg must not be const, it's not like copy-constructor at this point,
    // cause move semantics is going to take resources violently
    SomeThing(SomeThing &&other_thing) {
        std::cout << "[Special method invoked] ===> mov_copy constructor invoked, "
                  << "new obj address: "
                  << (void *) this << " copied__ obj address: "
                  << (void *) &other_thing << std::endl;

        this->a_ = other_thing.a_;
        this->chs_ = other_thing.chs_;
        other_thing.chs_ = nullptr;
        report();

        std::cout << "victim reports after being robbed: " << std::endl;
        other_thing.report();
    }

    // remember to handle self-assignment situation. no need to notice this in case of copy constructor
    // cause constructor always produce new obj.
    SomeThing &operator=(const SomeThing &other_thing) {
        std::cout << "[Special method invoked] ===> assign operator____ invoked, "
                  << "this address is: "
                  << (void *) this
                  << " assigned obj address: "
                  << (void *) &other_thing
                  << std::endl;

        if (this == &other_thing) {
            // it's not a constructor, so just return itself
            return *this;
        }

        this->a_ = other_thing.a_;
        this->DealWithChs(this->chs_, other_thing.chs_);
        report();
        return *this;
    }

    ~SomeThing() {
        std::cout << "[Special method invoked] ===> I am going to die, my address is: " << (void *) this << std::endl;
        delete[] chs_;
    }

    void report() {
        std::cout << "obj: " << (void *) this << " reports: internal a_ is: " << a_;

        if (chs_ != nullptr) {
            std::cout << " internal chs_ is: "
                      << chs_;
        }
        std::cout << std::endl;
    }
};

SomeThing LittleFunc(/* invoke copy constructor to initialize */SomeThing some_thing) {
    // invoke copy constructor to produce the return var
    return some_thing;
}

int main() {
    // default constructor
    {
        std::cout << "default constructor" << std::endl;
        SomeThing some_thing;
    }

    // type conversion constructor
    {
        std::cout << "implicit type conversion" << std::endl;
        SomeThing some_thing = 10;
    }

    // copy constructor
    {
        std::cout << "copy constructor" << std::endl;
        SomeThing some_thing(10);
        // invoke SomeThing copy constructor automatically
        SomeThing some_thing_new(some_thing);
    }

    // assign op
    {
        std::cout << "assign op" << std::endl;
        SomeThing some_thing(10);
        SomeThing some_thing_new;
        // invoke "operator ="
        some_thing_new = some_thing;
        some_thing_new = SomeThing(123);
    }

    // copy(with compiler optimization)
    {
        std::cout << "copy" << std::endl;
        // invoke normal constructor first to produce a temp obj (no name but has address and space)
        // , then invoke copy constructor to initialize some_thing.
        // Lastly, temp obj dies.
        SomeThing some_thing = SomeThing(10);
        SomeThing other_thing = some_thing; //2 form of copy constructor using
        SomeThing shit(other_thing);
    }

    // about function
    {
        std::cout << "about function" << std::endl;
        SomeThing some_thing;
        SomeThing some_thing_other;
        some_thing_other = LittleFunc(some_thing);
    }

    // ref and temp var
    {
        std::cout << "ref and temp var" << std::endl;
        // ref must be const, cause temp var is treated as a lvalue
        const SomeThing &some_thing_ref = SomeThing(10);

        // or using right value ref without const
        SomeThing &&some_thing_right_ref = SomeThing(100);
    }

    // move semantics. remember that move semantics is used to initialize an obj which means it always
    // used with move constructor. briefly, std::move is used in case of initializing new
    // obj instead of assignment situation.
    {
        std::cout << "move semantics" << std::endl;
        const char *str = "move semantic";
        SomeThing some_thing(str, strlen(str));
        SomeThing other_thing = std::move(some_thing);
        SomeThing anno_thing(std::move(other_thing));

        void ForMove(SomeThing);
        // copy, first the function ForMove need to produce a new obj for its formal param(
        // it's param is not a ref but a normal var), so some kind of constructor will be invoked.
        // then the real param is temp var which is a type of locator value, so copy constructor
        // will be invoked.
        ForMove(anno_thing);
        // move. invoke move constructor explicitly
        ForMove(std::move(anno_thing));
        // move. first the function ForMove need to produce a new obj for its formal param(
        // it's param is not a ref but a normal var), so some kind of constructor will be invoked.
        // then the real param is temp var which is a type of read only value, so move constructor
        // will be invoked. move semantics will fail if compiler optimization opened.
        ForMove(SomeThing(100));
    }
    return 0;
}

void ForMove(SomeThing some_thing) {}

