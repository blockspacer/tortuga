table! {
    historic_workers (id) {
        id -> Nullable<Integer>,
        uuid -> Text,
        worker_id -> Text,
        created -> Integer,
    }
}

table! {
    tasks (id) {
        id -> Nullable<Integer>,
        handle_id -> Text,
        task_type -> Text,
        data -> Binary,
        created -> Integer,
        max_retries -> Integer,
        retries -> Integer,
        priority -> Integer,
        delayed_time -> Nullable<Integer>,
        modules -> Nullable<Text>,
        worked_on -> Bool,
        worker_uuid -> Nullable<Text>,
        progress -> Float,
        progress_message -> Nullable<Text>,
        progress_metadata -> Nullable<Text>,
        status_code -> Nullable<Integer>,
        status_message -> Nullable<Text>,
        done -> Bool,
        started_time -> Nullable<Integer>,
        done_time -> Nullable<Integer>,
        logs -> Nullable<Text>,
        output -> Nullable<Text>,
    }
}

table! {
    workers (id) {
        id -> Nullable<Integer>,
        worker_id -> Text,
        uuid -> Text,
        capabilities -> Text,
        last_beat -> Integer,
        last_invalidated_uuid -> Nullable<Text>,
    }
}

allow_tables_to_appear_in_same_query!(
    historic_workers,
    tasks,
    workers,
);
