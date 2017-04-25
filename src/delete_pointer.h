template<typename T>
void delete_pointer (T *obj)
{
    if (obj != nullptr) {
        delete obj;
        obj = nullptr;
    }
}