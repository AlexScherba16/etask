#ifndef MISC_H
#define MISC_H
#include <functional>

namespace itask::utils::misc
{
    /**
     * @class Defer
     * @brief Utility class for executing a function upon destruction.
     *
     * The `Defer` class ensures that a given function is executed when
     * an instance of the class goes out of scope. Thanks to Golang for
     * naming.
     */

    class Defer
    {
    public:
        Defer() = delete;
        Defer(const Defer&) = delete;
        Defer(Defer&&) = delete;
        Defer& operator=(const Defer&) = delete;
        Defer& operator=(Defer&&) = delete;

        Defer(std::function<void()> func) :
            func_(std::move(func))
        {
        }

        ~Defer()
        {
            if (func_)
            {
                try
                {
                    func_();
                }
                catch (const std::exception& e)
                {
                    // log something
                }
                catch (...)
                {
                    // log something really terrible :)
                }
            }
        }

    private:
        std::function<void()> func_{};
    };
}

#endif //MISC_H
