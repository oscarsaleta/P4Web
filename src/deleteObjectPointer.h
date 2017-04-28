template<typename T>
void deleteObjectPointer (T *obj)
{
    if (obj != nullptr) {
        delete obj;
        obj = nullptr;
    }
}
