#!/usr/bin/env python3

import threading
import random
import time
import uuid

from queue import Queue, PriorityQueue, Empty
from enum import Enum


class SearchRequest:
    def __init__(self, query: str):
        self.query = query
        self.timestamp = time.time()

    def __str__(self) -> str:
        return f"SearchRequest({self.query}) {{ timestamp={self.timestamp} }}"


class ContentSource:
    def __init__(self, name: str, content: str):
        self.name = name
        self.content = content

    def __str__(self) -> str:
        return f"ContentSource({self.name})"


class SearchResult:
    def __init__(
        self,
        search_request: SearchRequest,
        content_source: ContentSource,
        line: int,
        column: int,
        length: int,
    ):
        self.search_request = search_request
        self.content_source = content_source
        self.line = line
        self.column = column
        self.length = length
        self.timestamp = time.time()
        self.provider = None

    def __str__(self) -> str:
        return f"SearchResult {{ content_source={self.content_source}, line={self.line}, column={self.column}, length={self.length}, timestamp={self.timestamp} }}"


class Message:
    pass


class NotEnoughCreditMessage(Message):
    pass


class SearchResultFoundMessage(Message):
    def __init__(self, search_request: SearchRequest, search_result: SearchResult):
        self.search_request = search_request
        self.search_result = search_result


class NoSearchResultsFoundMessage(Message):
    def __init__(self, search_request: SearchRequest):
        self.search_request = search_request


class CreditRechargeRequestMessage(Message):
    def __init__(self, client, amount: int):
        self.client = client
        self.amount = amount


class CreditRechargeResponseMessage(Message):
    def __init__(self, amount: int):
        self.amount = amount


class MessageReceiver:
    def push_message(self, message: Message):
        pass


class ClientSubscriptionType(Enum):
    FREE = 1
    PREMIUM = 2

    def __str__(self) -> str:
        return self.name


class Client(MessageReceiver):
    def __init__(
        self,
        subscription_type: ClientSubscriptionType,
        credit: int = -1,
        payments_service=None,
    ):
        self.id = uuid.uuid4()
        self.subscription_type = subscription_type
        self.credit = credit
        self.payments_service = payments_service

    @property
    def has_paid_subscription(self) -> bool:
        return (
            self.subscription_type == ClientSubscriptionType.PREMIUM
            and self.credit != -1
        )

    @property
    def has_credit(self) -> bool:
        return self.credit > 0

    def consume_credit(self, amount: int = 1):
        if self.has_credit:
            self.credit -= amount

    def push_message(self, message: Message):
        if isinstance(message, NotEnoughCreditMessage):
            print(f"no enough credit")
            payments_service.push_message(CreditRechargeRequestMessage(self, 15))
        elif isinstance(message, CreditRechargeResponseMessage):
            print(f"got {message.amount} in credit")
            self.credit += message.amount
        elif isinstance(message, NoSearchResultsFoundMessage):
            print(f"no results from {message.search_request}")
        elif isinstance(message, SearchResultFoundMessage):
            print(f"search result: {message.search_result}")

    def __str__(self) -> str:
        return f"Client({self.id}) {{ subscription_type={self.subscription_type} }}"


class SearchProvider:
    def query(self, client: Client, search_request: SearchRequest):
        pass


class SearchService(SearchProvider):
    def __init__(self):
        self.id = uuid.uuid4()
        self.content_sources = []

    def add_content_source(self, content_source: ContentSource):
        self.content_sources.append(content_source)

    def _query_st(
        self,
        content_source: ContentSource,
        client: Client,
        search_request: SearchRequest,
    ):
        result_count = 0
        lines = content_source.content.splitlines()
        for line_index in range(0, len(lines)):
            if client.has_paid_subscription and not client.has_credit:
                client.push_message(NotEnoughCreditMessage())
                while not client.has_credit:
                    # replace with cv
                    pass
                print("resuming query")

            time.sleep(0.1)
            string_index = lines[line_index].find(search_request.query)

            if string_index != -1:
                search_result = SearchResult(
                    search_request,
                    content_source,
                    1 + line_index,
                    1 + string_index,
                    len(search_request.query),
                )
                search_result.provider = self
                result_count += 1
                client.push_message(
                    SearchResultFoundMessage(search_request, search_result)
                )
                if client.has_paid_subscription:
                    client.consume_credit()

        if result_count == 0:
            client.push_message(NoSearchResultsFoundMessage(search_request))

    def query(self, client: Client, search_request: SearchRequest):
        threads = []
        for content_source in self.content_sources:
            thread = threading.Thread(
                target=self._query_st, args=(content_source, client, search_request)
            )
            thread.start()
            threads.append(thread)
        for thread in threads:
            thread.join()

    def __str__(self) -> str:
        return f"SearchService({self.id})"


class SearchProxy(SearchProvider):
    def __init__(self):
        self.search_services = []
        self.queues = {
            ClientSubscriptionType.FREE: PriorityQueue(),
            ClientSubscriptionType.PREMIUM: PriorityQueue(),
        }

    def add_search_service(self, search_service: SearchService):
        self.search_services.append(search_service)

    def query(self, client: Client, search_request: SearchRequest):
        self.queues[client.subscription_type].put(
            (1 / search_request.timestamp, (client, search_request))
        )

    def _spin_st(self, search_service: SearchService):
        """
        Single-thread Spin
        """
        try:
            p = random.random()
            if p <= 0.8:
                queue = self.queues[ClientSubscriptionType.PREMIUM]
            else:
                queue = self.queues[ClientSubscriptionType.FREE]

            priority, (client, search_request) = queue.get_nowait()
            random.choice(self.search_services).query(client, search_request)
        except Empty:
            pass

    def spin(self):
        threads = []
        for search_service in self.search_services:
            thread = threading.Thread(target=self._spin_st, args=(search_service,))
            thread.start()
            threads.append(thread)
        for thread in threads:
            thread.join()


class PaymentRequest:
    def __init__(self, client: Client, credits: int):
        self.client = client
        self.credits = credits


class PaymentsService(MessageReceiver):
    def __init__(self):
        self.queue = Queue()

    def push_message(self, message: Message):
        if isinstance(message, CreditRechargeRequestMessage):
            message.client.push_message(CreditRechargeResponseMessage(message.amount))

    def spin(self):
        try:
            payment_request = self.queue.get_nowait()
            payment_request.client.recharge_credit(amount=self.credits)
        except Empty:
            pass


class ClientFactory:
    @staticmethod
    def create_client(payments_service: PaymentsService) -> Client:
        client_subscription_type = random.choice(list(ClientSubscriptionType))
        client = Client(
            client_subscription_type,
            random.choice([-1, 10])
            if client_subscription_type == ClientSubscriptionType.PREMIUM
            else -1,
            payments_service,
        )
        return client


class SearchRequestFactory:
    @staticmethod
    def create_search_request() -> SearchRequest:
        return SearchRequest(random.choice(["hello", "world", "test"]))


if __name__ == "__main__":

    def search_request_mock_thread_proc(
        payments_service: PaymentsService,
        search_provider: SearchProvider,
        client_count: int = 1,
        search_request_count: int = 15,
        period: float = 2000,
    ):
        while True:
            clients = [
                ClientFactory.create_client(payments_service)
                for _ in range(0, client_count)
            ]
            for client in clients:
                search_requests = [
                    SearchRequestFactory.create_search_request()
                    for _ in range(0, search_request_count)
                ]
                for search_request in search_requests:
                    search_provider.query(client, search_request)
            time.sleep(period)

    def search_proxy_spin_thread_proc(search_proxy: SearchProxy):
        while True:
            search_proxy.spin()

    def payments_service_spin_thread_proc(payments_service: PaymentsService):
        while True:
            payments_service.spin()

    payments_service = PaymentsService()
    search_proxy = SearchProxy()
    content_sources = [
        ContentSource("source1", "hello world!\nthis is a test\nworld says hello"),
        ContentSource("source2", "world says goodbye\nhello says dave"),
        ContentSource(
            "source3", "oh my god does this work? this does\nwork!\ntest!!! TEST\n"
        ),
    ]
    search_services = [
        SearchService(),
        SearchService(),
        SearchService(),
        SearchService(),
    ]

    for search_service in search_services:
        search_service.content_sources = content_sources
        search_proxy.add_search_service(search_service)

    search_request_mock_thread = threading.Thread(
        target=search_request_mock_thread_proc,
        args=(
            payments_service,
            search_proxy,
        ),
    )
    search_proxy_spin_thread = threading.Thread(
        target=search_proxy_spin_thread_proc, args=(search_proxy,)
    )
    payments_service_spin_thread = threading.Thread(
        target=payments_service_spin_thread_proc, args=(payments_service,)
    )

    try:
        search_request_mock_thread.start()
        search_proxy_spin_thread.start()
        payments_service_spin_thread.start()
    except KeyboardInterrupt:
        pass
    finally:
        search_request_mock_thread.join()
        search_proxy_spin_thread.join()
        payments_service_spin_thread.join()
