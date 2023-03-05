#pragma once
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iterator>
#include <memory>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
	// Узел списка
	struct Node {
		Node() = default;
		Node(const Type& val, Node* next) :
			value(val),
			next_node(next)
		{}

		Type value;
		Node* next_node = nullptr;
	};

	template <typename ValueType>
	class BasicIterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = Type;
		using difference_type = std::ptrdiff_t;
		using pointer = ValueType*;
		using reference = ValueType&;

		BasicIterator() = default;

		BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

		BasicIterator& operator=(const BasicIterator& rhs) = default;

		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return node_ != rhs.node_;
		}

		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return node_ != rhs.node_;
		}

		BasicIterator& operator++() noexcept {
			node_ = node_->next_node;
			return *this;
		}

		BasicIterator operator++(int) noexcept {
			auto old_value(*this);
			++(*this);
			return old_value;
		}

		[[nodiscard]] reference operator*() const noexcept {
			return node_->value;
		}

		[[nodiscard]] pointer operator->() const noexcept {
			return &node_->value;
		}

	private:
		// Разрешаем SingleLinkedList обращаться к приватной области
		friend class SingleLinkedList;
		explicit BasicIterator(Node* node) : node_(node) {}
		Node* node_ = nullptr;
	};

public:
	using Iterator = BasicIterator<Type>;
	using ConstIterator = BasicIterator<const Type>;

	// Возвращает количество элементов в списке за время O(1)
	[[nodiscard]] size_t GetSize() const noexcept {
		return size_;
	}

	// Сообщает, пустой ли список за время O(1)
	[[nodiscard]] bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	void PushFront(const Type& value) {
		head_.next_node = new Node(value, head_.next_node);
		++size_;
	}

	void Clear() {
		Node* node = head_.next_node;
		while (node) {
			Node* next_node = node->next_node;
			delete node;
			--size_;
			node = next_node;
		}
		head_.next_node = nullptr;
	}

	[[nodiscard]] Iterator begin() noexcept { return Iterator{ head_.next_node }; }
	[[nodiscard]] Iterator end() noexcept { return Iterator{ nullptr }; }
	// Константные версии begin/end для обхода списка без возможности модификации его элементов
	[[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator{ head_.next_node }; }
	[[nodiscard]] ConstIterator end() const noexcept { return ConstIterator{ nullptr }; }
	// Методы для удобного получения константных итераторов у неконстантного контейнера
	[[nodiscard]] ConstIterator cbegin() const noexcept { return ConstIterator{ head_.next_node }; }
	[[nodiscard]] ConstIterator cend() const noexcept { return ConstIterator{ nullptr }; }

	// Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator before_begin() noexcept {
		return Iterator{ &head_ };
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		return ConstIterator{ const_cast<Node*>(&head_) };
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return ConstIterator{ &head_ };
	}

	/*
	 * Вставляет элемент value после элемента, на который указывает pos.
	 * Возвращает итератор на вставленный элемент
	 * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
	  */
	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		Node* new_node = new Node(value, pos.node_->next_node);
		pos.node_->next_node = new_node;
		++size_;
		return Iterator(new_node);
	}

	void PopFront() noexcept {
		Node* node_to_del = head_.next_node;
		head_.next_node = node_to_del->next_node;
		delete node_to_del;
		--size_;
	}

	/*
	 * Удаляет элемент, следующий за pos.
	 * Возвращает итератор на элемент, следующий за удалённым
	 */
	Iterator EraseAfter(ConstIterator pos) noexcept {
		Node* node_to_del = pos.node_->next_node;
		pos.node_->next_node = node_to_del->next_node;
		delete node_to_del;
		--size_;
		return Iterator{ pos.node_->next_node };
	}

	SingleLinkedList() {
		size_ = 0;
	}

	~SingleLinkedList() {
		Clear();
	}

	SingleLinkedList(std::initializer_list<Type> values) : size_(values.size()) {
		Node* node = &head_;
		for (const auto value : values) {
			node->next_node = new Node(value, nullptr);
			node = node->next_node;
		}
	}

	SingleLinkedList(const SingleLinkedList& other) : size_(other.GetSize()) {
		Node* node = &head_;
		for (const auto value : other) {
			node->next_node = new Node(value, nullptr);
			node = node->next_node;
		}
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		if (std::addressof(*this) != std::addressof(rhs)) {
			SingleLinkedList tmp = rhs;
			swap(tmp);
		}
		return *this;
	}

	// Обменивает содержимое списков за время O(1)
	void swap(SingleLinkedList& other) noexcept {
		auto tmp_head = head_.next_node;
		head_.next_node = other.head_.next_node;
		other.head_.next_node = tmp_head;

		auto tmp_size = size_;
		size_ = other.size_;
		other.size_ = tmp_size;
	}

private:
	// Фиктивный узел, используется для вставки "перед первым элементом"
	Node head_;
	size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	if (lhs.begin() == rhs.begin()) {
		return true;
	}

	if (lhs.GetSize() != rhs.GetSize()) {
		return false;
	}

	auto rhs_iter = rhs.cbegin();
	for (auto lhs_iter = lhs.cbegin(); lhs_iter != lhs.cend(); ++lhs_iter) {
		if (*lhs_iter != *rhs_iter) {
			return false;
		}
		++rhs_iter;
	}

	return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs == rhs) || (lhs < rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::greater());
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs == rhs) || (lhs > rhs);
}